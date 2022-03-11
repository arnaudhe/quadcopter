import time
from protocol import JsonProtocol, BinaryProtocol, ProtocolDecodeError
from broker import Broker
from typing import Any

class Controller:

    """
    This class implement data-model oriented commands, to remotely write and read quadcopter ressources.
    It relies on a UDP or radio broker, and register to their 'control' channel to send and receive commands.
    This base class must not instanciated, refer to UdpController and RadioController, depending
    on the broker kind we are using.
    """

    # Broker channel used for data model control operations
    CHANNEL = 'control'

    def __init__(self, broker: Broker):
        self._sequence = 0                                  # Next command sequence number
        self._broker = broker                               # Underlying udp or radio roker to send / receive commands
        self._broker.register_channel(Controller.CHANNEL)   # Register the channel to the broker to receive responses

    def _increment_sequence(self):
        """Increments command sequence number and handles roll-over"""
        self._sequence = (self._sequence + 1) % 256

    def _send_request_receive_response(self, request: dict) -> Any:
        """Serialize a command and send it. Then, wait a response with timeout and deserialize it"""
        # Serialize the request with the prococol and send it on the proper channel through the broker
        self._broker.send(Controller.CHANNEL, self._protocol.encode(request))
        for _ in range(30):
            if self._broker.received_frame_pending(Controller.CHANNEL):
                data = self._broker.receive(Controller.CHANNEL)
                try:
                    response = self._protocol.decode(data)  # try deserializing the response
                except ProtocolDecodeError:
                    print('[control] Invalid response. Drop it.')
                    continue    # Skip reponse handling
                if response['command'] == request['command'] and response['sequence'] == request['sequence'] and response['direction'] == 'response':
                    return response
                else:
                    print(f'[control] Not matching response {response["sequence"]}. Drop it.')
            else:
                time.sleep(0.1)
        print(f'[control] {request["command"]} response {request["sequence"]} timeout')

    def write(self, key: str, value: Any):
        """
        Remotely write a data model ressource.
        The key must be the data model ressource identifier (ex: "attitude.control.mode")
        The value must match the ressource type defined in the data model.
        For enum ressources, provide its string value respresentation
        """
        print(f'[control] write {key} {value}')
        self._increment_sequence()
        request = {'command' : 'write', 'sequence' : self._sequence, 'direction' : 'request', 'ressources' : {key : value}}
        response = self._send_request_receive_response(request)
        if response:
            if response['status'][key] == 'success':
                print('[control] Success')
            else:
                print('[control] Error status {}'.format(response['status'][key]))

    def read(self, key: str) -> tuple:
        """
        Remotely read a data model ressource.
        The key must be the data model ressource identifier (ex: "attitude.control.mode")
        This returns a tuple with read command status (True or False) and the corresponding read value
        in case of read success, None otherwise
        """
        print(f'[control] read {key}')
        self._increment_sequence()
        request = {'command' : 'read', 'sequence' : self._sequence, 'direction' : 'request', 'ressources' : [key]}
        response = self._send_request_receive_response(request)
        if response:
            print(response['ressources'][key])
            return True, response['ressources'][key]
        else:
            return False, None

class UdpController(Controller):
    """
    UDP specification of base class Controller
    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Instanciate JSON protocol for command serialization 
        self._protocol = JsonProtocol()

class RadioController(Controller):
    """
    Radio specification of base class Controller
    """

    def __init__(self, data_model, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Instanciate binary protocol for command serialization
        self._protocol = BinaryProtocol(data_model)
