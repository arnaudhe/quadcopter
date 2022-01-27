
from threading import Thread
import sys
import socket
import time
import json

class QuadcopterController:

    ROLL_PITCH_TRIM_STEP   = 0.005
    YAW_THROTTLE_TRIM_STEP = 0.005
    STICK_SCALE = 500000

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
                    roll_trim = roll_trim - QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['B']:
                    roll_trim = roll_trim + QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['A']:
                    pitch_trim = pitch_trim - QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['Y']:
                    pitch_trim = pitch_trim + QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['left']:
                    yaw_trim = yaw_trim - QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['right']:
                    yaw_trim = yaw_trim + QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['down']:
                    throttle_trim = throttle_trim - QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['up']:
                    throttle_trim = throttle_trim + QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['left_trigger'] and state['keys']['right_trigger']:
                    armed = not armed
                    if armed:
                        self.write('control.attitude.roll.mode', 'position')
                        self.write('control.attitude.pitch.mode', 'position')
                        self.write('control.attitude.yaw.mode', 'speed')
                        self.write('control.mode', 'attitude')
                    else:
                        self.write('control.mode', 'off')
                roll_stick     = state['right_stick']['horizontal'] / QuadcopterController.STICK_SCALE
                pitch_stick    = state['right_stick']['vertical']   / QuadcopterController.STICK_SCALE
                yaw_stick      = state['left_stick']['horizontal']  / QuadcopterController.STICK_SCALE
                throttle_stick = state['left_stick']['vertical']    / QuadcopterController.STICK_SCALE
                if roll_trim + roll_stick != roll:
                    roll = roll_trim + roll_stick
                    self.write('control.attitude.roll.position.target', roll)
                if pitch_trim + pitch_stick != pitch:
                    pitch = pitch_trim + pitch_stick
                    self.write('control.attitude.pitch.position.target', pitch)
                if yaw_trim + yaw_stick != yaw:
                    yaw = yaw_trim + yaw_stick
                    self.write('control.attitude.yaw.speed.target', yaw)
                if throttle_trim + throttle_stick != throttle:
                    throttle = throttle_trim + throttle_stick
                    self.write('control.attitude.height.manual.throttle', throttle)
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
