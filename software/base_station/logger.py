import time
from datetime import datetime
from worker import ThreadedWorker

class Logger(ThreadedWorker):

    """
    This class registers to logs channel, and displays received logs
    """

    CHANNEL = 'logs'

    def __init__(self, broker):
        super().__init__()
        self._broker = broker
        self._broker.register_channel(Logger.CHANNEL)

    def _run(self):
        """Internal read loop"""
        print('[logger] Started')
        while self.running():
            if self._broker.received_frame_pending(Logger.CHANNEL):
                data = self._broker.receive(Logger.CHANNEL)
                print(f'[{Logger.CHANNEL}] {datetime.now()} {data.decode("utf-8")}')
            else:
                time.sleep(0.1)
        print('[logger] Stopped')

if __name__ == '__main__':

    from udp import UdpBroker

    broker = UdpBroker()
    logger = Logger(broker)
    logger.start()
    while True:
        try:
            time.sleep(0.1)
        except KeyboardInterrupt:
            break
    logger.stop()
    broker.stop()
