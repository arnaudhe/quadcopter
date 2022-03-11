import socket
from queue import Queue
from worker import ThreadedWorker
from broker import Broker, ChannelListener
from typing import Tuple

class UdpChannelListener(ThreadedWorker, ChannelListener):
    """
    This class implements a channel listener for a given communication channel
    It listens data on a UDP socket and expects data specific to the corresponding
    channel to be sent on corresponding UDP port
    Listener does not manage data sending 
    """

    def __init__(self, name: str, port: int, address_filtering: str = None):
        ThreadedWorker.__init__(self)
        ChannelListener.__init__(self, address_filtering)
        self._name = name
        self._port = port
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def _run(self):
        """Internal read loop"""
        while self.running():
            try:
                data, remote = self._sock.recvfrom(1024)
                if data:
                    self.enqueue(remote[0], data)
            except socket.timeout:
                pass
    
    def start(self):
        """Start UDP listener"""
        self._sock.settimeout(0.1)
        self._sock.bind(('', self._port))
        print(f'[{self._name}] Socket bound on port {self._port}')
        ThreadedWorker.start(self)

    def stop(self):
        """Stop UDP listener"""
        ThreadedWorker.stop(self)
        self._sock.close()
        print(f'[{self._name}] Socket closed')

class UdpBroker(Broker):
    """
    The class is the specification of broker for UDP-based communication
    Listeners and sending are based on UDP sockets (a listening socket per registered channel)
    Channel multiplexing is done with UDP sockets port
    """

    PORT_BASE = 5000

    def __init__(self):
        super().__init__()
        self._send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def _create_listener(self, channel: str) -> UdpChannelListener:
        """Create a UDP listener"""
        udp_port = UdpBroker.PORT_BASE + Broker.CHANNELS[channel]
        return UdpChannelListener(channel, udp_port, self._quadcopter_address)

    def _send_to(self, address: str, channel: str, data: bytes):
        """Send a frame to the requested destination address"""
        udp_port = UdpBroker.PORT_BASE + Broker.CHANNELS[channel]
        try:
            self._send_sock.sendto(data, (address, udp_port))
        except:
            print('[UDP Broker] Send error')
