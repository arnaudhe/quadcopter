from threading import Thread
import time

class Hearbeat(Thread):

    """
    This class implement quadcopter detection and address retrieving, based on heartbeat frames periodically sent
    by the quadcopter.
    It relies on a UDP or radio broker, and register to their 'heartbeat' channel to receive quadcopter advertisement heartbeats.
    When quadcopter detected, use this class to get the quacopter address to be able to send it commands
    """

    CHANNEL = 'heartbeat'
    PERIOD = 1.0

    def __init__(self, broker):
        super().__init__(target=self.run)
        self.broker = broker
        self.broker.register_channel(Hearbeat.CHANNEL)
        self.running = True
        self.detected = False
        self.quadcoter_address = None
        self.quadcoter_name = None
        self.quadcopter_heartbeat = 5

    def wait_heartbeat(self, blocking = True):
        result = False
        if blocking:
            print('[heartbeat] Waiting for heartbeat')
        while True:
            try:
                if self.broker.received_frame_pending(Hearbeat.CHANNEL):
                    address, data = self.broker.receive(Hearbeat.CHANNEL)
                    self.quadcopter_address = address
                    self.quadcopter_name = data.decode('utf-8')
                    result = True
                else:
                    time.sleep(Hearbeat.PERIOD)
            except KeyboardInterrupt:
                result = False
                break
            if (blocking == False) or (result == True):
                break
        return result

    def run(self):
        print('[heartbeat] Started')
        while self.running:
            if self.wait_heartbeat(blocking = False):
                if self.quadcopter_heartbeat == 0:
                    print(f'[heartbeat] Retrieved signal from {self.quadcopter_name}!')
                self.quadcopter_heartbeat = 5
            else:
                if self.quadcopter_heartbeat > 0:
                    self.quadcopter_heartbeat -= 1
                    if self.quadcopter_heartbeat == 0:
                        print(f'[heartbeat] Lost signal from {self.quadcopter_name}')
        print('[heartbeat] Stopped')

    def stop(self):
        self.running = False
        self.join()

    def get_quadcopter_name(self):
        return self.quadcopter_name

    def get_quadcopter_address(self):
        return self.quadcopter_address
