import socket
import time
from datetime import datetime
from threading import Thread

class UdpLogger(Thread):

    def __init__(self):
        super().__init__()
        self.running = True
        self.start()

    def run(self):
        print('[LOGGER] started')
        udp_logger = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        udp_logger.bind(('0.0.0.0', 5002))
        udp_logger.settimeout(0.1)
        while self.running:
            try:
                data = udp_logger.recv(128)
                if data:
                    print(f'[LOGGER] {datetime.now()} {data.decode("utf-8")}')
            except:
                pass
        print('[LOGGGER] stopped')

    def shutdown(self):
        self.running = False
        self.join()

if __name__ == '__main__':

    logger = UdpLogger()
    while True:
        try:
            time.sleep(0.1)
        except KeyboardInterrupt:
            break
    logger.shutdown()