import json, os, select, socket
import subprocess, threading, time, queue
import urllib.request
from tqdm import tqdm

class YiActionCamSettings:

    DEFAULT_SETTINGS = {'ip_addr': '192.168.42.1',
                        'ctrl_port': 7878,
                        'http_port': 80,
                        'data_port': 8787,
                        'vlc_path': '"/Applications/VLC.app/Contents/MacOS/VLC"',
                        'chunk_size': 65536,
                        'download_folder': 'files',
                        'media_path_absolute': '/tmp/fuse_d/DCIM/100MEDIA',
                        'media_path_relative': 'DCIM/100MEDIA',
                        'filename': ''}

    def __init__(self, **settings):
        self._dict = YiActionCamSettings.DEFAULT_SETTINGS
        self._dict.update(settings)
        self.load()

    def load(self):
        if self.filename:
            with open(self.filename, 'r') as infile:
                self._dict.update(json.load(infile))

    def save(self):
        if self.filename:
            with open(self.filename, 'w') as outfile:
                json.dump(self._dict, outfile)

    def __getattribute__(self, name):
        if name != '_dict' and self._dict and name in self._dict:
            return self._dict[name]
        else:
            return super().__getattribute__(name)

    def __setattr__(self, name, value) -> None:
        if name != '_dict' and self._dict and name in self._dict:
            self._dict[name] = value
        else:
            super().__setattr__(name, value)

class DownloadProgressBar(tqdm):

    def update_to(self, b=1, bsize=1, tsize=None):
        if tsize is not None:
            self.total = tsize
        self.update(b * bsize - self.n)

class YiActionCam:

    COMMAND_REBOOT = 2
    COMMAND_READ_CONFIG = 3
    COMMAND_FORMAT = 4
    COMMAND_GET_SPACE = 5
    COMMAND_BATTERY = 7
    COMMAND_GET_BATTERY = 13
    COMMAND_AUTHENT = 257
    COMMAND_OPEN_PORTS = 259
    COMMAND_START_RECORDING = 513
    COMMAND_STOP_RECORDING = 514
    COMMAND_PHOTO = 770
    COMMAND_LIST_DIRECTORY = 1282
    COMMAND_CHANGE_DIRECTORY = 1283
    COMMAND_EXECUTE_SCRIPT = 1286

    def __init__(self, **settings):
        self.connected   = False
        self.token       = 0
        self.settings    = YiActionCamSettings(**settings)
        self.recv_buffer = {}
        self.recv_queue  = queue.Queue()
        self.config      = {}

    def read_loop(self):
        json_data = b""
        json_counter = 0
        json_flip = 0
        self.ctrl_client.setblocking(0)
        print('Start read loop')
        while self.connected:
            try:
                ready = select.select([self.ctrl_client], [], [])
                if ready[0]:
                    byte = self.ctrl_client.recv(1)
                    if byte == b"{":
                        json_counter += 1
                        json_flip = 1
                    elif byte == b"}":
                        json_counter -= 1
                    json_data += byte
                    if json_flip == 1 and json_counter == 0:
                        json_data = json_data.decode("utf-8")
                        print(f'Received json data : {json_data}')
                        try:
                            data_dec = json.loads(json_data)
                            self.recv_queue.put(data_dec)
                        except json.JSONDecodeError:
                            print('Bad json data !')
                        json_data = b""
            except Exception as e:
                print('Error during read loop !', e)
                self.connected = False

    def start_read_thread(self):
        self.thread_read = threading.Thread(target=self.read_loop)
        self.thread_read.setDaemon(True)
        self.thread_read.setName('read_loop')
        self.thread_read.start()

    def wait_command(self, command, timeout=3.0):
        wait = 0.0
        while self.recv_buffer[command] == None:
            if self.recv_queue.empty() == False:
                print('Processing')
                recv_data = self.recv_queue.get()
                self.recv_buffer[recv_data['msg_id']] = recv_data
            time.sleep(0.1)
            wait += 0.1
            if wait > timeout:
                raise Exception(f'wait_command {command} timed out')
        return self.recv_buffer[command]

    def send_command(self, command, additional = {}):
        data = {'msg_id': command, 'token': self.token}
        data.update(additional)
        print(f'Send command {json.dumps(data)}')
        self.ctrl_client.send(json.dumps(data).encode('utf-8'))

    def send_recv_command(self, command, additional={}, timeout=3.0):
        self.send_command(command, additional)
        self.recv_buffer[command] = None
        return self.wait_command(command, timeout)

    def authent(self):
        self.token = self.send_recv_command(YiActionCam.COMMAND_AUTHENT)['param']
        print(f"Token refreshed {self.token}")

    def get_human_readable_size(self, size):
        units = ["kB", "MB", "GB", "TB"]
        unit = 0
        size = float(size)
        while size > 1024.0:
            size = size / 1024.0
            unit += 1
        return "%.1f%s" % (size, units[unit])

    def update_usage(self):
        self.total_space = self.send_recv_command(YiActionCam.COMMAND_GET_SPACE, {'type' : 'total'})["param"]
        self.free_space = self.send_recv_command(YiActionCam.COMMAND_GET_SPACE, {'type': 'free'})["param"]
        self.used_space = self.total_space - self.free_space
        print('Used {} / {}'.format(self.get_human_readable_size(self.used_space),
                                    self.get_human_readable_size(self.total_space)))

    def update_battery(self):
        resp = self.send_recv_command(YiActionCam.COMMAND_GET_BATTERY)
        if resp["type"] == "adapter":
            self.charging = True
        else:
            self.charging = False
        self.battery_level = resp["param"]
        print(f'Battery {self.battery_level}, charging = {self.charging}')

    def take_picture(self):
        self.send_command(YiActionCam.COMMAND_PHOTO)
        self.update_usage()

    def stop_recording(self):
        self.send_command(YiActionCam.COMMAND_STOP_RECORDING)
        self.update_usage()

    def start_recording(self):
        self.send_command(YiActionCam.COMMAND_START_RECORDING)
    
    def start_live(self):
        self.send_command(YiActionCam.COMMAND_OPEN_PORTS, {"param": "none_force"})
        to_run = f'{self.settings.vlc_path} rtsp://{self.settings.ip_addr}:554/live'
        subprocess.Popen(to_run, shell=True)

    def change_directory(self, directory):
        self.pwd = self.send_recv_command(YiActionCam.COMMAND_CHANGE_DIRECTORY, {'param': directory})['pwd']

    def get_directory(self):
        self.change_directory('.')
        print(f'Current directory is {self.pwd}')
        return self.pwd

    def list_directory(self, directory = '.'):
        old = self.pwd
        self.change_directory(directory)
        resp = self.send_recv_command(YiActionCam.COMMAND_LIST_DIRECTORY, {"param":" -D -S"})
        self.list_files = []
        self.list_directories = []
        for node in resp["listing"]:
            if not list(node.keys())[0].endswith("/"):
                self.list_files.append(node)
            else:
                self.list_directories.append(node)
        self.change_directory(old)
        return self.list_files

    def execute_script(self, script, md5sum, offset = 0, size = 0):
        self.send_command(YiActionCam.COMMAND_EXECUTE_SCRIPT, {"param": script, "offset": 0, "size": 0, "md5sum": md5sum})

    def enable_debug(self):
        self.execute_script("enable_info_display.script", "d41d8cd98f00b204e9800998ecf8427e")

    def reboot(self):
        print('Reboot asked. Exiting TCP session')
        self.send_command(YiActionCam.COMMAND_REBOOT, {"type": "dev_reboot", "param": "on"})
        self.connected = False  # Exit

    def factory_reset(self):
        print('Factory reset asked. Exiting TCP session')
        self.send_command(YiActionCam.COMMAND_REBOOT, {"type": "restore_factory_settings", "param": "on"})
        self.connected = False  # Exit

    def enable_telnet(self):
        self.get_directory()
        self.change_directory('/tmp/fuse_d')
        self.enable_debug()
        self.reboot()

    def format(self):
        self.send_command(YiActionCam.COMMAND_FORMAT)
        time.sleep(1)
        self.update_usage()

    def download_thread(self):
        self.send_command(YiActionCam.COMMAND_OPEN_PORTS, {"param": "none_force"})
        if not os.path.isdir(self.settings.download_folder):
            os.mkdir(self.settings.download_folder, 0o777)
        try:
            download_url = f"http://{self.settings.ip_addr}:{self.settings.http_port}/{self.settings.media_path_relative}/{self.download_file}"
            print(f'Download url is {download_url}')
            with DownloadProgressBar(unit='B', unit_scale=True, miniters=1, desc=self.download_file) as t:
                urllib.request.urlretrieve(download_url, filename=os.path.join(self.settings.download_folder, self.download_file), reporthook=t.update_to)
        except Exception as e:
            print('Files download failed')
            print(e)

    def download(self, file):
        self.download_file = file
        self.thread_file_download = threading.Thread(target=self.download_thread)
        self.thread_file_download.start()

    def take_and_download_photo(self):
        self.take_picture()
        time.sleep(1)
        self.list_directory(self.settings.media_path_absolute)
        if len(self.list_files):
            last_file = self.list_files[-1]
            last_filename = list(last_file.keys())[0]
            print(last_filename)
            self.download(last_filename)
        else:
            print('Empty media file')

    def read_config_all(self):
        resp = self.send_recv_command(YiActionCam.COMMAND_READ_CONFIG)
        for each in resp["param"]:
            self.config.update(each)

    def read_config_param(self, param):
        resp = self.send_recv_command(YiActionCam.COMMAND_READ_CONFIG, {'param': param})
        for each in resp["param"]:
            self.config.update(each)

    def get_info(self):
        return "Firmware version: %s\nHardware version: %s\nSerial Number:    %s" % (self.config["sw_version"], self.config["hw_version"], self.config["serial_number"])

    def connect(self):
        try:
            socket.setdefaulttimeout(5)
            self.ctrl_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #create socket
            self.ctrl_client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.ctrl_client.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            self.ctrl_client.connect((self.settings.ip_addr, self.settings.ctrl_port))  # open socket
            self.start_read_thread()
            self.connected = True
            self.authent()
            self.get_directory()
            self.update_battery()
            self.update_usage()
            self.read_config_all()
        except Exception as e:
            print(e)
            print('Failed to connect !')
            self.connected = False
            self.ctrl_client.close()

    def is_connected(self):
        return self.connected

if __name__ == "__main__":

    camera = YiActionCam()
    camera.connect()

    camera.take_and_download_photo()
    camera.update_usage()

    while camera.is_connected():
        time.sleep(10)
        camera.update_battery()
