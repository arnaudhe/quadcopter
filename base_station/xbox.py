from struct import pack
import usb
import construct
import math
import threading
import time

class XBoxController:

    STICK_DEAD_ZONE = 7000

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
            'horizontal' / construct.Int16sl,
            'vertical' / construct.Int16sl,
        ),
        'right_stick' / construct.Struct (
            'horizontal' / construct.Int16sl,
            'vertical' / construct.Int16sl,
        )
    )

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
                    packet['left_stick']['horizontal']  = 0 if (math.fabs(packet['left_stick']['horizontal'])  < XBoxController.STICK_DEAD_ZONE) else packet['left_stick']['horizontal']
                    packet['left_stick']['vertical']    = 0 if (math.fabs(packet['left_stick']['vertical'])    < XBoxController.STICK_DEAD_ZONE) else packet['left_stick']['vertical']
                    packet['right_stick']['horizontal'] = 0 if (math.fabs(packet['right_stick']['horizontal']) < XBoxController.STICK_DEAD_ZONE) else packet['right_stick']['horizontal']
                    packet['right_stick']['vertical']   = 0 if (math.fabs(packet['right_stick']['vertical'])   < XBoxController.STICK_DEAD_ZONE) else packet['right_stick']['vertical']
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

