import time
import socket
import json
from protocol import JsonProtocol, BinaryProtocol, ProtocolDecodeError

class Controller:

    """
    This class implement data-model oriented commands, to remotely write and read quadcopter ressources.
    It relies on a UDP or radio broker, and register to their 'control' channel to send and receive commands.
    This base class must not instanciated, refer to UdpController and RadioController, depending
    on the broker kind we are using.
    Refer to Heartbeat to be able to dynamically detect quadcopter address from advertising.
    """

    CHANNEL = 'control'

    def __init__(self, broker, quadcopter_address):
        self.sequence = 0
        self.broker = broker
        self.quadcopter_address = quadcopter_address
        self.broker.register_channel(Controller.CHANNEL)

    def increment_sequence(self):
        self.sequence = (self.sequence + 1) % 256

    def send_request_receive_response(self, request):
        self.broker.send(self.quadcopter_address, Controller.CHANNEL, self.protocol.encode(request))
        for _ in range(30):
            if self.broker.received_frame_pending(Controller.CHANNEL):
                _, data = self.broker.receive(Controller.CHANNEL)
                try:
                    response = self.protocol.decode(data)
                except ProtocolDecodeError:
                    print('[control] Invalid response. Drop it.')
                    continue
                if response['command'] == request['command'] and response['sequence'] == request['sequence'] and response['direction'] == 'response':
                    return response
                else:
                    print(f'[control] Not matching response {response["sequence"]}. Drop it.')
            else:
                time.sleep(0.1)
        print(f'[control] {request["command"]} response {request["sequence"]} timeout')

    def write(self, key, value):
        print(f'[control] write {key} {value}')
        self.increment_sequence()
        request = {'command' : 'write', 'sequence' : self.sequence, 'direction' : 'request', 'ressources' : {key : value}}
        response = self.send_request_receive_response(request)
        if response:
            if response['status'][key] == 'success':
                print('[control] Success')
            else:
                print('[control] Error status {}'.format(response['status'][key]))

    def read(self, key):
        print(f'[control] read {key}')
        self.increment_sequence()
        request = {'command' : 'read', 'sequence' : self.sequence, 'direction' : 'request', 'ressources' : [key]}
        response = self.send_request_receive_response(request)
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
        self.protocol = JsonProtocol()

class RadioController(Controller):

    """
    Radio specification of base class Controller
    """

    def __init__(self, data_model, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.protocol = BinaryProtocol(data_model)
