from typing import Any, Dict, Tuple
from queue import Queue

class ChannelListener:

    """
    Abstract class defining the interface for a channel listener
    This class is specified for udp and radio channel listeners
    """

    def __init__(self, address_filtering: Any = None):
        self._queue = Queue()
        self._address_filtering = address_filtering

    def enqueue(self, address: Any, data: bytes):
        """Externally insert received data into the listener queue"""
        if (address == self._address_filtering) or (self._address_filtering == None):
            self._queue.put((address, data))

    def set_address_filtering(self, address: str):
        """Activate address filtering on received frames"""
        self._address_filtering = address

    def start(self):
        """Start the listener"""
        pass

    def receive(self) -> Tuple[Any, bytes]:
        """
        If received frame pending, returns a tuple of (source address, frame)
        Make first sure to have a pending received frame with received_frame_pending
        """
        if self.received_frame_pending():
            return self._queue.get()

    def received_frame_pending(self) -> bool:
        """
        Returns True if at least one pending frame has been received
        Returns False otherwise
        """
        return not self._queue.empty()

    def stop(self):
        """Stop the listener"""
        pass

class Broker:
    """
    The class defines the interface for a communication broker
    This class must not be instanciated, use UdpBroker or RadioBroker specification
    It handles "channel" multiplexing over a same physical communication medium
    Thoses channels are pure software, they don't correspond to physical channels
    """

    # Common definitions of communication channels
    CHANNELS = dict(heartbeat=0, control=1, radio_command=2, telemetry=3, logs=4)

    def __init__(self):
        self._listeners: Dict[str, ChannelListener] = {}
        self._started = False
        self._quadcopter_address = None

    def _create_listener(self, channel: str):
        """Abstract method overloaded by specified broker classes"""
        return ChannelListener()

    def start(self):
        """Start the broker and all its underlying registered listeners"""
        if not self._started:
            self._started = True
            for channel in self._listeners:
                self._listeners[channel].start()

    def stop(self):
        """Stop the broker and all its underlying registered listeners"""
        if self._started:
            self._started = False
            for channel in self._listeners:
                self._listeners[channel].stop()

    def register_channel(self, channel: str):
        """Register a channel in reception"""
        if channel in Broker.CHANNELS and not self.is_registered(channel):
            self._listeners[channel] = self._create_listener(channel)
            print(f'[Broker] Channel {channel} registered')
            if self._started:
                print(f'[Broker] Start listener {channel}')
                self._listeners[channel].start()

    def is_registered(self, channel: str) -> bool:
        """Returns if a channel has been registered and has a listener attached"""
        return channel in self._listeners

    def set_quadcopter_address(self, address: Any):
        """
        Activate address filtering on received frames and enables frame sending
        This function might been called after heartbeat detection, with
        quadcopter address detected by heartbeat
        """
        print('[Broker] Enable address filtering')
        self._quadcopter_address = address
        for channel in self._listeners:
            self._listeners[channel].set_address_filtering(address)

    def receive_from(self, channel: str) -> Tuple[Any, bytes]:
        """
        Returns a tuple containing (source_address, data) when frame received on the
        required channel. Returns nothing otherwise. Check if data received first with
        method received_frame_pending
        """
        if self.received_frame_pending(channel):
            address, data = self._listeners[channel].receive()
            return address, data

    def receive(self, channel: str) -> bytes:
        """
        Returns received frame (bytes) on the required channel.
        Returns nothing if not data received.
        Check if data received first with method received_frame_pending.
        """
        if self.received_frame_pending(channel):
            # Discard address
            _, data = self._listeners[channel].receive()
            return data

    def received_frame_pending(self, channel: str) -> bool:
        """
        Returns True if a frame has been received on the provided channel
        Returns False otherwise, or if the channel has not been registered or is invalid
        """
        if self.is_registered(channel):
            return self._listeners[channel].received_frame_pending()
        else:
            return False

    def send(self, channel: str, data: bytes):
        """Send data to the provided address on the requested channel"""
        if self._quadcopter_address:
            self._send_to(self._quadcopter_address, channel, data)
