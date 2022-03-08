import socket
import time
from datetime import datetime
from threading import Thread

class Logger(Thread):

    CHANNEL = 'logs'

    def __init__(self, broker):
        super().__init__()
        self.broker = broker
        self.running = True
        self.broker.register_channel(Logger.CHANNEL)
        self.start()

    def run(self):
        print(f'[{Logger.CHANNEL}] Started')
        while self.running:
            if self.broker.received_frame_pending(Logger.CHANNEL):
                _, data = self.broker.receive(Logger.CHANNEL)
                print(f'[{Logger.CHANNEL}] {datetime.now()} {data.decode("utf-8")}')
            else:
                time.sleep(0.1)
        print(f'[{Logger.CHANNEL}] Stopped')

    def stop(self):
        self.running = False
        self.join()

if __name__ == '__main__':

    from udp import UdpBroker

    broker = UdpBroker()
    logger = Logger(broker)
    while True:
        try:
            time.sleep(0.1)
        except KeyboardInterrupt:
            break
    logger.stop()
    broker.stop()
