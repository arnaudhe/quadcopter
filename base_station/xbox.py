import usb.core as Usb
from construct import Adapter, Struct, BitStruct, Int16sl, Byte, Bit, Int8ub, Padding
from threading import Lock
from worker import ThreadedWorker

class XboxStickValue(Adapter):

    """
    This class handles adaptation between raw stick value 16-bit signed integer
    and normalized value in interval [-1.0, 1.0]
    It also handles stick dead zone removing, as the value does not go back to zero
    when releasing the stick at its center position
    """

    STICK_DEAD_ZONE = 8000      # raw value unit
    STICK_MAX_VALUE = 32768     # 16-bit signed integer

    def __init__(self):
        super().__init__(Int16sl)

    def _encode(self, obj, context, path):
        """
        Overload of construct.Adapter "_encode" method.
        Transforms normalized value to raw value
        """
        if obj > 0.0:
            return (int)(obj * XboxStickValue.STICK_MAX_VALUE) + XboxStickValue.STICK_DEAD_ZONE
        else:
            return (int)(obj * XboxStickValue.STICK_MAX_VALUE) - XboxStickValue.STICK_DEAD_ZONE

    def _decode(self, obj, context, path):
        """
        Overload of construct.Adapter "_decode" method.
        Transforms raw value to normalized value, with dead zone removing
        """
        value = obj
        if value < 0:
            if value > -XboxStickValue.STICK_DEAD_ZONE:
                value = 0
            else:
                value = value + XboxStickValue.STICK_DEAD_ZONE
        else:
            if value < XboxStickValue.STICK_DEAD_ZONE:
                value = 0
            else:
                value = value - XboxStickValue.STICK_DEAD_ZONE
        return value / XboxStickValue.STICK_MAX_VALUE

class XBoxController(ThreadedWorker):

    """
    This class aims at getting current state of a plugged USB Xbox controller
    It works with low-level USB interface, requiring any driver attached to driver
    to be unload first, to be able to access the device
    """

    PACKET = Struct(
        'type' / Byte,
        'length' / Byte,
        'keys' / BitStruct(
            Padding(2),
            'select' / Bit,
            'start' / Bit,
            'right' / Bit,
            'left' / Bit,
            'down' / Bit,
            'up' / Bit,
            'Y' / Bit,
            'X' / Bit,
            'B' / Bit,
            'A' / Bit,
            Padding(1),
            'xbox' / Bit,
            'right_trigger' / Bit,
            'left_trigger' / Bit,
        ),
        'left_push' / Int8ub,
        'right_push' / Int8ub,
        'left_stick' / Struct (
            'horizontal' / XboxStickValue(),
            'vertical' / XboxStickValue(),
        ),
        'right_stick' / Struct (
            'horizontal' / XboxStickValue(),
            'vertical' / XboxStickValue(),
        )
    )

    @staticmethod
    def detect() -> bool:
        """Returns True if at least one USB xbox controller has been detected"""
        if Usb.find(idVendor = 1118, idProduct = 654):
            return True
        else:
            return False

    def __init__(self):
        super().__init__()
        self._dev = Usb.find(idVendor = 1118, idProduct = 654)
        self._dev.set_configuration()
        self._read_endpoint  = self._dev[0][(0,0)][0]    # endpoint to read from
        self._write_endpoint = self._dev[0][(0,0)][1]    # endpoint to write to
        self._lock  = Lock()                    # thread-safe lock for shared state sttribute
        self._state = {}

    def _run(self):
        """Internal read loop"""
        while self.running():
            try:
                data = self._dev.read(self._read_endpoint.bEndpointAddress, self._read_endpoint.wMaxPacketSize, 100)
                if len(data) == 20:
                    packet = XBoxController.PACKET.parse(data)
                    self._lock.acquire()
                    self._state = packet
                    del self._state['type']
                    del self._state['length']
                    self._lock.release()
            except Usb.USBError:
                pass

    def get_state(self) -> dict:
        """Returns current remote controller state"""
        self._lock.acquire()
        state_copy = self._state
        self._lock.release()
        return state_copy

if __name__ == '__main__':

    from time import sleep

    if XBoxController.detect():
        controller = XBoxController()
        controller.start()
        while True:
            try:
                state = controller.get_state()
                if state:
                    print(controller.get_state())
                sleep(0.1)
            except KeyboardInterrupt:
                break
        controller.stop()
    else:
        print('Not detected any xbox controller')
