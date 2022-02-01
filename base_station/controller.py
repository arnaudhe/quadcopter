
from threading import Thread
import sys
import socket
import time
import json
import struct

class QuadcopterController:

    ROLL_PITCH_TRIM_STEP   = 0.01
    YAW_THROTTLE_TRIM_STEP = 0.005
    THURST_YAW_STICK_SCALE = 400000
    ROLL_PITCH_STICK_SCALE = 500000

    def __init__(self, data_model):
        self.sequence = 0
        self.data_model = data_model
        self.model_loaded = False
        self.quadcopter_address = ''
        self.quadcopter_name = ''
        self.quadcopter_heartbeat = 5
        print('[CTRL] Waiting for quadcopter announcement...')
        res = self.wait_announcement(blocking = True)
        if not res:
            sys.exit(1)
        print(f'[CTRL] Detected announcement from {self.quadcopter_name} at {self.quadcopter_address}')
        self.running = True
        self.heartbeat_task = Thread(target = self.run_heartbeat)
        self.heartbeat_task.start()
        self.xbox_controller = None

    def attach_xbox_controller(self, xbox_controller):
        self.xbox_controller = xbox_controller
        self.xbox_task = Thread(target = self.run_xbox)
        self.xbox_task.start()

    def get_data_model(self):
        return self.data_model

    def shutdown(self):
        self.running = False
        self.heartbeat_task.join()
        if self.xbox_controller:
            self.xbox_task.join()

    def run_heartbeat(self):
        print('[HEARTBEAT] Started')
        while self.running:
            if self.wait_announcement(blocking = False):
                if self.quadcopter_heartbeat == 0:
                    print(f'[HEARTBEAT] Retrieved signal from {self.quadcopter_name}!')
                self.quadcopter_heartbeat = 5
            else:
                if self.quadcopter_heartbeat > 0:
                    self.quadcopter_heartbeat -= 1
                    if self.quadcopter_heartbeat == 0:
                        print(f'[HEARTBEAT] Lost signal from {self.quadcopter_name}')
        print('[HEARTBEAT] Stopped')

    def run_xbox(self):
        print('[REMOTE] Started')
        armed = False
        arming = False
        roll_trim = 0.0
        pitch_trim = 0.0
        yaw_trim = 0.0
        throttle_trim = 0.4
        roll = 0.0
        pitch = 0.0
        yaw = 0.0
        throttle = 0.3
        while self.running:
            state = self.xbox_controller.get_state()
            if state:
                if state['keys']['X']:
                    roll_trim = roll_trim + QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['B']:
                    roll_trim = roll_trim - QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['A']:
                    pitch_trim = pitch_trim + QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['Y']:
                    pitch_trim = pitch_trim - QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['left']:
                    yaw_trim = yaw_trim - QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['right']:
                    yaw_trim = yaw_trim + QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['down']:
                    throttle_trim = throttle_trim - QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['up']:
                    throttle_trim = throttle_trim + QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['left_trigger'] and state['keys']['right_trigger']:
                    if not arming:
                        arming = True
                        armed = not armed
                else:
                    if arming:
                        arming = False
                roll_stick     = -state['right_stick']['horizontal'] / QuadcopterController.ROLL_PITCH_STICK_SCALE
                pitch_stick    = -state['right_stick']['vertical']   / QuadcopterController.ROLL_PITCH_STICK_SCALE
                yaw_stick      =  state['left_stick']['horizontal']  / QuadcopterController.THURST_YAW_STICK_SCALE
                throttle_stick =  state['left_stick']['vertical']    / QuadcopterController.THURST_YAW_STICK_SCALE
                roll     = roll_trim     + roll_stick
                pitch    = pitch_trim    + pitch_stick
                yaw      = yaw_trim      + yaw_stick
                throttle = throttle_trim + throttle_stick
                self.write_multi(armed, roll, pitch, yaw, throttle)
            time.sleep(0.1)
        print('[REMOTE] Stopped')

class QuadcopterUdpController(QuadcopterController):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.udp_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def wait_announcement(self, blocking = False):
        # Bind a new socket on 5001 to detect sent announcements
        udp_detector = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        udp_detector.bind(('0.0.0.0', 5001))
        udp_detector.settimeout(1.1)
        result = False
        while True:
            try:
                data, address = udp_detector.recvfrom(128)
                message = json.loads(data.decode('utf-8'))
                if 'announcement' in message:
                    self.quadcopter_address, _ = address
                    self.quadcopter_name = message["announcement"]
                    result = True
            except KeyboardInterrupt:
                result = False
                break
            except Exception as e:
                pass
            if (blocking == False) or (result == True):
                break
        udp_detector.close()
        return result

    def flush(self):
        self.udp_client.setblocking(False)
        try:
            while len(self.udp_client.recv(1024)):
                pass
        except:
            pass
        self.udp_client.setblocking(True)
        self.udp_client.settimeout(2.0)

    def write(self, key, value):
        print(f'[UDP CTRL] write {key} {value}')
        self.sequence = (self.sequence + 1) % 256
        command = {'command' : 'write', 'sequence' : self.sequence, 'direction' : 'request', 'ressources' : {key : value}}
        self.flush()
        self.udp_client.sendto(json.dumps(command).encode('utf-8'), (self.quadcopter_address, 5000))
        try:
            data = self.udp_client.recv(1024).decode("utf-8")
        except socket.timeout:
            print(f'[UDP CTRL] Response timeout when writing ressource {key}')
            return
        try:
            response = json.loads(data)
        except:
            print(f'[UDP CTRL] Invalid response')
            return
        if (response['command'] == "write" and response['sequence'] == self.sequence and response['direction'] == 'response'):
            if response['status'][key] == 'success':
                print('[UDP CTRL] Success')
            else:
                print('[UDP CTRL] Error status {}'.format(response['status'][key]))
        else:
            print('[UDP CTRL] Not matching response {}. Drop it.'.format(response['sequence']))

    def read(self, key):
        print(f'[UDP CTRL] read {key}')
        self.sequence = (self.sequence + 1) % 256
        command = {'command' : 'read', 'sequence' : self.sequence, 'direction' : 'request', 'ressources' : [key]}
        self.flush()
        self.udp_client.sendto(json.dumps(command).encode('utf-8'), (self.quadcopter_address, 5000))
        try:
            data = self.udp_client.recv(1024).decode("utf-8")
        except socket.timeout:
            print(f'[UDP CTRL] Response timeout when reading ressource {key}')
            return False, None
        try:
            response = json.loads(data)
        except:
            print(f'[UDP CTRL] Invalid response')
            return False, None
        if (response['command'] == "read" and response['sequence'] == self.sequence and response['direction'] == 'response'):
            if response['status'][key] == 'success':
                print(response['ressources'][key])
                return True, response['ressources'][key]
            else:
                print('[UDP CTRL] Error status {}'.format(response['status'][key]))
                return False, None
        else:
            print('[UDP CTRL] Not matching response {} vs {}. Drop it.'.format(response['sequence'], self.sequence))
            return False, None

    def write_multi(self, armed, roll, pitch, yaw, throttle):
        data = bytes([int(armed)]) + struct.pack("ffff", roll, pitch, yaw, throttle)
        try:
            self.udp_client.sendto(data, (self.quadcopter_address, 5005))
        except:
            pass

class QuadcopterRadioController(QuadcopterController):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def wait_announcement(self, blocking = False):
        # TODO
        return False

    def write(self, key, value):
        print(f'[RADIO CTRL] write {key} {value}')
        self.sequence = (self.sequence + 1) % 256
        # TODO
        return

    def read(self, key):
        print(f'[RADIO CTRL] read {key}')
        self.sequence = (self.sequence + 1) % 256
        # TODO
        return False, None
