import socket
from queue import Queue
from threading import Thread

class Udp(Thread):

    def __init__(self, channel, port, read = True):
        super().__init__(target=self.read_loop)
        self.channel = channel
        self.port    = port
        self.sock    = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.queue   = Queue()
        self.running = False
        if read:
            self.sock.settimeout(1.0)
            self.sock.bind(('', port))
            print(f'[{self.channel}] Socket bound on port {port}')
            self.running = True
            self.start()

    def read_loop(self):
        while self.running:
            try:
                data, address = self.sock.recvfrom(1024)
                if len(data):
                    self.queue.put((address[0], data))
            except socket.timeout:
                pass

    def receive(self):
        if self.received_frame_pending():
            return self.queue.get()

    def received_frame_pending(self):
        return not self.queue.empty()

    def send(self, address, data):
        try:
            self.sock.sendto(data, (address, self.port))
        except:
            print(f'[{self.channel}] Send error')

    def stop(self):
        if self.running:
            self.running = False
            self.join()
        self.sock.close()
        print(f'[{self.channel}] Socket closed')

class UdpBroker:

    CHANNELS = dict(heartbeat=5000, control=5001, radio_command=5002, telemetry=5003, logs=5004)

    def __init__(self):
        self._registrations = {}

    def register_channel(self, channel, read = True):
        if channel in UdpBroker.CHANNELS:
            self._registrations[channel] = Udp(channel, UdpBroker.CHANNELS[channel], read)

    def receive(self, channel):
        if self.received_frame_pending(channel):
            return self._registrations[channel].receive()

    def received_frame_pending(self, channel):
        if channel in self._registrations:
            return self._registrations[channel].received_frame_pending()
        else:
            return False

    def send(self, address: str, channel: str, data: bytes):
        if channel in self._registrations:
            self._registrations[channel].send(address, data)

    def stop(self):
        for udp in self._registrations:
            self._registrations[udp].stop()
