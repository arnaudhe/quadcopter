from xbox import XBoxController
from threading import Thread
import time
import struct

class RadioCommand(Thread):

    CHANNEL = 'radio_command'

    ROLL_PITCH_TRIM_STEP   = 0.01
    YAW_THROTTLE_TRIM_STEP = 0.005
    THURST_YAW_STICK_SCALE = 400000
    ROLL_PITCH_STICK_SCALE = 500000

    def __init__(self, xbox_controller, broker, quadcopter_address):
        super().__init__()
        self.xbox_controller = xbox_controller
        self.broker = broker
        self.broker.register_channel(RadioCommand.CHANNEL, read=False)
        self.quadcopter_address = quadcopter_address
        self.running = True
        self.start()

    def run(self):
        print('[radio_command] Started')
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
                    roll_trim = roll_trim + RadioCommand.ROLL_PITCH_TRIM_STEP
                if state['keys']['B']:
                    roll_trim = roll_trim - RadioCommand.ROLL_PITCH_TRIM_STEP
                if state['keys']['A']:
                    pitch_trim = pitch_trim + RadioCommand.ROLL_PITCH_TRIM_STEP
                if state['keys']['Y']:
                    pitch_trim = pitch_trim - RadioCommand.ROLL_PITCH_TRIM_STEP
                if state['keys']['left']:
                    yaw_trim = yaw_trim - RadioCommand.YAW_THROTTLE_TRIM_STEP
                if state['keys']['right']:
                    yaw_trim = yaw_trim + RadioCommand.YAW_THROTTLE_TRIM_STEP
                if state['keys']['down']:
                    throttle_trim = throttle_trim - RadioCommand.YAW_THROTTLE_TRIM_STEP
                if state['keys']['up']:
                    throttle_trim = throttle_trim + RadioCommand.YAW_THROTTLE_TRIM_STEP
                if state['keys']['left_trigger'] and state['keys']['right_trigger']:
                    if not arming:
                        arming = True
                        armed = not armed
                else:
                    if arming:
                        arming = False
                roll_stick     = -state['right_stick']['horizontal'] / RadioCommand.ROLL_PITCH_STICK_SCALE
                pitch_stick    = -state['right_stick']['vertical']   / RadioCommand.ROLL_PITCH_STICK_SCALE
                yaw_stick      =  state['left_stick']['horizontal']  / RadioCommand.THURST_YAW_STICK_SCALE
                throttle_stick =  state['left_stick']['vertical']    / RadioCommand.THURST_YAW_STICK_SCALE
                roll     = roll_trim     + roll_stick
                pitch    = pitch_trim    + pitch_stick
                yaw      = yaw_trim      + yaw_stick
                throttle = throttle_trim + throttle_stick
                self.send(armed, roll, pitch, yaw, throttle)
            time.sleep(0.1)
        print('[radio_command] Stopped')

    def stop(self):
        self.running = False
        self.join()

    def send(self, armed, roll, pitch, yaw, throttle):
        payload = bytes([int(armed)]) + struct.pack("ffff", roll, pitch, yaw, throttle)
        self.broker.send(self.quadcopter_address, RadioCommand.CHANNEL, payload)
