from xbox import XBoxController
from worker import PeriodicWorker
from broker import Broker
import struct

class RadioCommand(PeriodicWorker):

    """
    This class get user commands from a remote controller (xbox) and
    stream those commands to the quadcopter through the "radio_command"
    broker channel
    """

    CHANNEL = 'radio_command'

    TRIM_STEP = 0.01

    RATES = {
        'roll'     : (-1, 0.2, 50, 140),
        'pitch'    : (-1, 0.2, 50, 140),
        'yaw'      : (-1, 1.0, 50, 140),
        'throttle' : ( 1, 1.2, 50, 140)
    }

    def __init__(self, xbox_controller: XBoxController, broker: Broker):
        super().__init__(0.1)
        self._xbox_controller = xbox_controller
        self._broker          = broker
        self._armed           = False
        self._arming          = False
        self._roll_trim       = -0.03
        self._pitch_trim      = 0.02
        self._yaw_trim        = 0.0
        self._throttle_trim   = 0.0
        self._broker.register_channel(RadioCommand.CHANNEL)

    def _compute_rate(self, raw: float, axis: str):
        """
        Transform the raw stick position to a quadcopter, by applying a
        non-linear rate to be more sensitive on small sticks moves 
        """
        sens, rate, expo, acro = RadioCommand.RATES[axis]
        command = sens * ((1 + 0.01 * expo * (raw * raw - 1.0)) * raw)
        return (command * (rate + (abs(command) * rate * acro * 0.01)))
    
    def _send(self, armed: bool, roll: float, pitch: float, yaw: float, throttle: float):
        """
        Serialize setpoints into radio command frame and send it through radio command broker channel
        """
        payload = bytes([int(armed)]) + struct.pack("ffff", roll, pitch, yaw, throttle)
        self._broker.send(RadioCommand.CHANNEL, payload)

    def _run(self):
        """
        Internal periodic run. Get remote controller commands, compute setpoints and send them
        through radio command broker channel
        """
        state = self._xbox_controller.get_state()
        if state:
            if state['keys']['X']:
                self._roll_trim = self._roll_trim - RadioCommand.TRIM_STEP
            if state['keys']['B']:
                self._roll_trim = self._roll_trim + RadioCommand.TRIM_STEP
            if state['keys']['A']:
                self._pitch_trim = self._pitch_trim - RadioCommand.TRIM_STEP
            if state['keys']['Y']:
                self._pitch_trim = self._pitch_trim + RadioCommand.TRIM_STEP
            if state['keys']['left']:
                self._yaw_trim = self._yaw_trim - RadioCommand.TRIM_STEP
            if state['keys']['right']:
                self._yaw_trim = self._yaw_trim + RadioCommand.TRIM_STEP
            if state['keys']['down']:
                self._throttle_trim = self._throttle_trim - RadioCommand.TRIM_STEP
            if state['keys']['up']:
                self._throttle_trim = self._throttle_trim + RadioCommand.TRIM_STEP
            if state['keys']['left_trigger'] and state['keys']['right_trigger']:
                if not self._arming:
                    self._arming = True
                    self._armed = not self._armed
            else:
                if self._arming:
                    self._arming = False
            roll = self._compute_rate(self._roll_trim + state['right_stick']['horizontal'], 'roll')
            pitch = self._compute_rate(self._pitch_trim + state['right_stick']['vertical'], 'pitch')
            yaw = self._compute_rate(self._yaw_trim + state['left_stick']['horizontal'], 'yaw')
            throttle = self._compute_rate(self._throttle_trim + state['left_stick']['vertical'], 'throttle')
            self._send(self._armed, roll, pitch, yaw, throttle)
