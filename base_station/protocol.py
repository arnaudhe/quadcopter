import json
from construct import Struct, BitStruct, BitsInteger, Adapter, Int8ul, Int32sl, Float32l, Enum, Byte, Flag, Float64l, Switch, Array, this

class ProtocolEncodeError(Exception):

    pass

class ProtocolDecodeError(Exception):

    pass

class BinaryConvertionError(Exception):

    def __init__(self, message):
        super().__init__(message)

class BinaryConvertionKeyError(BinaryConvertionError):

    def __init__(self, key):
        super().__init__('Invalid key :', key)

class BinaryConvertionTypeError(BinaryConvertionError):

    def __init__(self, type_1, type_2):
        super().__init__('Type mismatch :', type_1, type_2)

class KeyStruct(Adapter):
    """
    This class handles adaptation between string key ("attitude.control.mode") and binary id ([0, 1, 2])
    To do so, its contructor receives a dictionary (str, list[int]) of keys to id translation 
    """

    def __init__(self, ressources_id):
        self._ressources_id = ressources_id
        super().__init__(Struct(
            'length'  / Byte,
            'fields'  / Array(this.length, Int8ul)
        ))

    def _decode(self, obj, context, path):
        for key in self._ressources_id:
            if self._ressources_id[key] == list(obj['fields']):
                return key
        raise BinaryConvertionKeyError(obj['fields'])

    def _encode(self, obj, context, path):
        try:
            ressource_id = self._ressources_id[obj]
        except KeyError:
            raise BinaryConvertionKeyError(obj)
        return dict(length=len(ressource_id), fields=ressource_id)

class RessourcesStruct(Adapter):
    """
    This class handles ressources tree format adaptation
    """

    STATUS_ENUM = Enum(Byte, success=0, not_found=1, bad_type=2, bad_value=3, bad_permissions=4)

    def __init__(self, ressources_type, ressources_enum: dict, ressources_id):
        self._ressources_type = ressources_type
        self._ressources_struct = {}
        for key, value_enum in ressources_enum.items():
            enum = Enum(Byte, **value_enum)
            value_struct = Switch(this.type, {'integer':Int32sl, 'float':Float32l, 'enum':enum, 'bool':Flag, 'status':RessourcesStruct.STATUS_ENUM, 'double':Float64l})
            self._ressources_struct[key] = value_struct
        super().__init__(Array(this.header.count,
            Struct (
                'key'   / KeyStruct(ressources_id),
                'type'  / Enum(Byte, empty=0, integer=1, float=2, enum=3, bool=4, status=5, double=6),
                'value' / Switch(this.key, self._ressources_struct)
            )
        ))

    def _extract_ressources_keys(self, obj):
        return [ressource['key'] for ressource in obj]

    def _extract_ressources_key_values(self, obj):
        ressources = {}
        for ressource in obj:
            if ressource['type'] == 'enum' or ressource['type'] == 'status':
                value = str(ressource['value'])
            else:
                value = ressource['value']
            ressources[ressource['key']] = value
        return ressources

    def _decode(self, obj, context, path):
        command = context['header']['command']
        direction = context['header']['direction']
        if (command, direction) in (('read', 'request'), ('notify', 'request')):
            # thoses requests only contains a list of keys
            return self._extract_ressources_keys(obj)
        else:
            return self._extract_ressources_key_values(obj)

    def _encode(self, obj, context, path):
        command = context['header']['command']
        direction = context['header']['direction']
        if (command, direction) in (('read', 'request'), ('notify', 'request')):
            # thoses requests only contain a list of keys
            return [dict(key=key, type='empty', value=None) for key in obj]
        elif (command, direction) in (('write', 'response'), ('notify', 'response')):
            # thoses responses contain status values
            return [dict(key=key, type='status', value=obj[key]) for key in obj]
        else:
            return [dict(key=key, type=self._ressources_type[key], value=obj[key]) for key in obj]

class BinaryProtocolStruct(Adapter):
    """
    This class is the higher level for binary frame building and parsing
    It defines the global frame structure and performs adaptation between binary-level structure and
    applicative-level dictionary structure
    """

    def __init__(self, data_model):
        self._ressources_id = {}
        self._ressources_type = {}
        self._ressources_enum = {}
        self._load_ressources(data_model)
        super().__init__(Struct (
            'header' / BitStruct(
                'command'   / Enum(BitsInteger(3), read=1, write=2, notify=3),
                'direction' / Enum(BitsInteger(1), request=0, response=1),
                'count'     / BitsInteger(4),
                'sequence'  / BitsInteger(8)
            ),
            'ressources'    / RessourcesStruct(self._ressources_type, self._ressources_enum, self._ressources_id)
        ))

    def _load_ressources(self, data_model, key = '', bin_id = []):
        for name, content in data_model.items():
            if isinstance(content, dict):
                child_key = key + '.' + name if key else name
                child_id = bin_id + [content['id']]
                if 'type' in content:
                    # This is a leaf, register type, enum values and binary id
                    enum_values = content['values'] if content['type'] == 'enum' else dict()
                    self._ressources_type[child_key] = content['type']
                    self._ressources_id[child_key] = child_id
                    self._ressources_enum[child_key] = enum_values
                else:
                    # Recursion
                    self._load_ressources(content, child_key, child_id)

    def _decode(self, obj, context, path):
        command = str(obj['header']['command'])
        direction = str(obj['header']['direction'])
        sequence = obj['header']['sequence']
        tree = dict(command=command, direction=direction, sequence=sequence)
        if (command, direction) in (('write', 'response'), ('notify', 'response')):
            tree['status'] = obj['ressources']
        else:
            tree['ressources'] = obj['ressources']
        return tree

    def _encode(self, obj, context, path):
        if (obj['command'], obj['direction']) in (('write', 'response'), ('notify', 'response')):
            ressources = obj['status']
        else:
            ressources = obj['ressources']
        return dict(header=dict(command=obj['command'], direction=obj['direction'],
                                sequence=obj['sequence'], count=len(ressources)), 
                    ressources=ressources)

class JsonProtocol:

    def __init__(self):
        super().__init__()

    def encode(self, command):
        try:
            return json.dumps(command).encode('utf-8')
        except:
            raise ProtocolEncodeError('Json Protocol Encoding Error')

    def decode(self, payload):
        try:
            return json.loads(payload)
        except json.JSONDecodeError:
            raise ProtocolDecodeError('Json Protocol Decoding Error')

class BinaryProtocol:

    def __init__(self, data_model):
        super().__init__()
        self.struct = BinaryProtocolStruct(data_model)

    def encode(self, command):
        try:
            return self.struct.build(command)
        except:
            raise ProtocolEncodeError('Binary Protocol Encoding Error')

    def decode(self, payload):
        try:
            return self.struct.parse(payload)
        except json.JSONDecodeError:
            raise ProtocolDecodeError('Binary Protocol Decoding Error')

if __name__ == '__main__':

    import pprint
    import os

    with open(os.path.join(os.path.dirname(__file__), os.path.pardir, 'data_model', 'data_model.json')) as json_file:
        data_model = json.load(json_file)

    protocol = BinaryProtocolStruct(data_model)

    TEST_COMMANDS = [ "431903010301023e99999a05010401020102412000000203010302",
                      "32aa02010203030202010164000000",
                      "22ab0201020002020100",
                      "52ac02010205000202010503",
                      "431903010301023e99999a05010401020202412000000203010302"]

    for command in TEST_COMMANDS:
        tree = protocol.parse(bytes.fromhex(command))
        pprint.pprint(tree)
        hex_back = protocol.build(tree).hex()
        print(hex_back)
        assert(hex_back == command)

    lulu = {'command' : 'write',
            'direction' : 'request',
            'sequence' : 25,
            'ressources' : {
                'control.motors.front_left' : 0.3,
                'control.attitude.roll.position.current' : 10.0,
                'wifi.status' : 'disconnected'
            }}

    print(protocol.build(lulu).hex())

    lulu = {'command' : 'write',
            'direction' : 'response',
            'sequence' : 26,
            'status' : {
                'control.motors.front_left' : 'success',
                'control.attitude.roll.position.current' : 'not_found',
                'wifi.status' : 'bad_type'
            }}

    print(protocol.build(lulu).hex())

    lulu = {'command' : 'read',
            'direction' : 'request',
            'sequence' : 27,
            'ressources' : [
                'control.motors.front_left',
                'control.attitude.roll.position.current',
                'wifi.status'
            ]}

    print(protocol.build(lulu).hex())

    lulu = {'command' : 'notify',
            'direction' : 'response',
            'sequence' : 28,
            'status' : {
            }}

    print(protocol.build(lulu).hex())
