import usb
import construct
import math
import threading
import time

class XboxStickValue(construct.Adapter):

    STICK_DEAD_ZONE = 8000
    STICK_MAX_VALUE = 32768

    def __init__(self):
        super().__init__(construct.Int16sl)

    def _encode(self, obj, context, path):
        if obj > 0.0:
            return (int)(obj * XboxStickValue.STICK_MAX_VALUE) + XboxStickValue.STICK_DEAD_ZONE
        else:
            return (int)(obj * XboxStickValue.STICK_MAX_VALUE) - XboxStickValue.STICK_DEAD_ZONE

    def _decode(self, obj, context, path):
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

class XBoxController:

    PACKET = construct.Struct(
        'type' / construct.Byte,
        'length' / construct.Byte,
        'keys' / construct.BitStruct(
            construct.Padding(2),
            'select' / construct.Bit,
            'start' / construct.Bit,
            'right' / construct.Bit,
            'left' / construct.Bit,
            'down' / construct.Bit,
            'up' / construct.Bit,
            'Y' / construct.Bit,
            'X' / construct.Bit,
            'B' / construct.Bit,
            'A' / construct.Bit,
            construct.Padding(1),
            'xbox' / construct.Bit,
            'right_trigger' / construct.Bit,
            'left_trigger' / construct.Bit,
        ),
        'left_push' / construct.Int8ub,
        'right_push' / construct.Int8ub,
        'left_stick' / construct.Struct (
            'horizontal' / XboxStickValue(),
            'vertical' / XboxStickValue(),
        ),
        'right_stick' / construct.Struct (
            'horizontal' / XboxStickValue(),
            'vertical' / XboxStickValue(),
        )
    )

    @staticmethod
    def detect():
        if usb.core.find(idVendor = 1118, idProduct = 654):
            return True
        else:
            return False

    def __init__(self):
        self.dev = usb.core.find(idVendor = 1118, idProduct = 654)
        self.dev.set_configuration()
        self.read_end_point  = self.dev[0][(0,0)][0]    # endpoint to read from
        self.write_end_point = self.dev[0][(0,0)][1]    # endpoint to write to
        self.running = False
        self.lock = threading.Lock()
        self.thread = threading.Thread(target = self.read_loop)
        self.state = {}
    
    def read_loop(self):
        while self.running:
            try:
                data = self.dev.read(self.read_end_point.bEndpointAddress, self.read_end_point.wMaxPacketSize, 100)
                if len(data) == 20:
                    packet = XBoxController.PACKET.parse(data)
                    self.lock.acquire()
                    self.state = packet
                    del self.state['type']
                    del self.state['length']
                    self.lock.release()
            except usb.core.USBError:
                pass

    def start(self):
        self.running = True
        self.thread.start()

    def stop(self):
        self.running = False
        self.thread.join()

    def get_state(self):
        self.lock.acquire()
        state_copy = self.state
        self.lock.release()
        return state_copy

if __name__ == '__main__':
    if XBoxController.detect():
        controller = XBoxController()
        controller.start()
        while True:
            try:
                state = controller.get_state()
                if state:
                    print(controller.get_state())
                time.sleep(0.1)
            except KeyboardInterrupt:
                break
        controller.stop()
    else:
        print('Not detected any xbox controller')
