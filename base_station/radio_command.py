from xbox import XBoxController
from threading import Thread
import time
import struct

class RadioCommand(Thread):

    CHANNEL = 'radio_command'

    TRIM_STEP = 0.01

    RATES = {
        'roll' : (-1, 0.2, 50, 140),
        'pitch' : (-1, 0.2, 50, 140),
        'yaw' : (-1, 1.0, 50, 140),
        'throttle' : (1, 1.2, 50, 140)
    }

    def __init__(self, xbox_controller, broker, quadcopter_address):
        super().__init__()
        self.xbox_controller = xbox_controller
        self.broker = broker
        self.broker.register_channel(RadioCommand.CHANNEL)
        self.quadcopter_address = quadcopter_address
        self.running = True
        self.start()

    def compute_rate(self, raw, axis):
        sens, rate, expo, acro = RadioCommand.RATES[axis]
        command = sens * ((1 + 0.01 * expo * (raw * raw - 1.0)) * raw)
        return (command * (rate + (abs(command) * rate * acro * 0.01)))

    def run(self):
        print('[radio_command] Started')
        armed           = False
        arming          = False
        roll_trim       = -0.03
        pitch_trim      = 0.02
        yaw_trim        = 0.0
        throttle_trim   = 0.0
        while self.running:
            state = self.xbox_controller.get_state()
            if state:
                if state['keys']['X']:
                    roll_trim = roll_trim - RadioCommand.TRIM_STEP
                if state['keys']['B']:
                    roll_trim = roll_trim + RadioCommand.TRIM_STEP
                if state['keys']['A']:
                    pitch_trim = pitch_trim - RadioCommand.TRIM_STEP
                if state['keys']['Y']:
                    pitch_trim = pitch_trim + RadioCommand.TRIM_STEP
                if state['keys']['left']:
                    yaw_trim = yaw_trim - RadioCommand.TRIM_STEP
                if state['keys']['right']:
                    yaw_trim = yaw_trim + RadioCommand.TRIM_STEP
                if state['keys']['down']:
                    throttle_trim = throttle_trim - RadioCommand.TRIM_STEP
                if state['keys']['up']:
                    throttle_trim = throttle_trim + RadioCommand.TRIM_STEP
                if state['keys']['left_trigger'] and state['keys']['right_trigger']:
                    if not arming:
                        arming = True
                        armed = not armed
                else:
                    if arming:
                        arming = False
                roll     = self.compute_rate(roll_trim + state['right_stick']['horizontal'], 'roll')
                pitch    = self.compute_rate(pitch_trim + state['right_stick']['vertical'], 'pitch')
                yaw      = self.compute_rate(yaw_trim + state['left_stick']['horizontal'], 'yaw')
                throttle = self.compute_rate(throttle_trim + state['left_stick']['vertical'], 'throttle')
                print(armed, roll, pitch, yaw, throttle)
                self.send(armed, roll, pitch, yaw, throttle)
            time.sleep(0.05)
        print('[radio_command] Stopped')

    def stop(self):
        self.running = False
        self.join()

    def send(self, armed, roll, pitch, yaw, throttle):
        payload = bytes([int(armed)]) + struct.pack("ffff", roll, pitch, yaw, throttle)
        self.broker.send(self.quadcopter_address, RadioCommand.CHANNEL, payload)
