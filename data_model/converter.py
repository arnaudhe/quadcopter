import json
from construct import *
import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLineEdit, QMainWindow, QTextEdit, QVBoxLayout
from PyQt5.QtGui import QFont
import pprint

class HexString:

    @staticmethod
    def is_hex(hex_string):
        try:
            bytearray.fromhex(hex_string)
            return True
        except:
            return False

    @staticmethod
    def from_bytes(input_bytes):
        output = ''
        for b in input_bytes:
            output = output + hex(b)[2:].zfill(2)
        return output

    @staticmethod
    def to_bytes(hex_string, expected_len = 0):
        if (expected_len != 0 and ((len(hex_string) / 2) != expected_len)):
            raise ValueError('Invalid len')
        return bytes(bytearray.fromhex(hex_string))

class DataProtocolConverter:

    STATUS_ENUM = Enum(Byte, success=0, not_found=1, bad_type=2, bad_value=3, bad_permissions=4)

    RESOURCE_STRUCT = Struct (
        'id'      / Struct (
            'length'  / Byte,
            'fields'  / Array(this.length, Int8ul)
        ),
        'type'    / Enum(Byte, empty=0, integer=1, float=2, enum=3, bool=4, status=5),
        'value'   / Switch(this.type, {'integer':Int32sl, 'float':Float32b, 'enum':Enum(Byte), 'bool':Flag, 'status':STATUS_ENUM})
    )

    FRAME_STRUCT = Struct (
        'header'        / BitStruct(
            'command'       / Enum(BitsInteger(3), read=1, write=2, notify=3),
            'direction'     / Enum(BitsInteger(1), request=0, response=1),
            'count'         / BitsInteger(4),
            'sequence'      / BitsInteger(8)
        ),
        'ressources'    / Array(this.header.count, RESOURCE_STRUCT)
    )

    @staticmethod
    def get_type(id_str, data_model):
        id_str_list = id_str.split('.')
        if len(id_str_list) == 1:
            return data_model[id_str_list[0]]['type']
        else:
            return DataProtocolConverter.get_type('.'.join(id_str_list[1:]), data_model[id_str_list[0]])

    @staticmethod
    def get_enum_values(id_str, data_model):
        id_str_list = id_str.split('.')
        if len(id_str_list) == 1:
            try:
                return data_model[id_str_list[0]]['values']
            except:
                return {}
        else:
            return DataProtocolConverter.get_enum_values('.'.join(id_str_list[1:]), data_model[id_str_list[0]])

    @staticmethod
    def get_value(id_str, value, data_model):
        ressource_type = DataProtocolConverter.get_type(id_str, data_model)
        enum_values = DataProtocolConverter.get_enum_values(id_str, data_model)
        struct = Switch(ressource_type, { 'integer':Int32sl, 'float':Float32b, 'enum':Enum(Byte, **enum_values), 'bool':Flag, 'status':DataProtocolConverter.STATUS_ENUM})
        return struct.parse(struct.build(value))

    @staticmethod
    def encode_id(id_str, data_model, current = None):
        if current is None:
            current = []
        id_str_list = id_str.split('.')
        current.append(data_model[id_str_list[0]]['id'])
        if len(id_str_list) == 1:
            return current
        else:
            return DataProtocolConverter.encode_id('.'.join(id_str_list[1:]), data_model[id_str_list[0]], current)

    @staticmethod
    def decode_id(id_list, data_model, current = ''):
        for name, content in data_model.items():
            if isinstance(content, dict):
                if content['id'] == id_list[0]:
                    if len(id_list) == 1:
                        return current + name
                    else:
                        return DataProtocolConverter.decode_id(id_list[1:], data_model[name], current + name + '.')
        raise Exception("Invalid id")

    @staticmethod
    def decode(buffer, data_model):
        tree = DataProtocolConverter.FRAME_STRUCT.parse(buffer)
        ressources = {}
        for ressource in tree['ressources']:
            key = DataProtocolConverter.decode_id(ressource['id']['fields'], data_model)
            value = DataProtocolConverter.get_value(key, ressource['value'], data_model) if ressource['type'] == 'enum' else ressource['value']
            value = str(value) if ressource['type'] == 'status' or ressource['type'] == 'enum' else value
            ressources[key] = value
        if tree['header']['command'] == 'read' and tree['header']['direction'] == 'request':
            ressources = list(ressources.keys())
        if tree['header']['command'] == 'write' and tree['header']['direction'] == 'response':
            ressources_key = 'status'
        else:
            ressources_key = 'ressources'
        return {'command' : str(tree['header']['command']),
                'direction' : str(tree['header']['direction']),
                'sequence' : tree['header']['sequence'],
                ressources_key : ressources}

    @staticmethod
    def encode(tree, data_model):
        if 'ressources' in tree:
            length = len(tree['ressources'])
            if isinstance(tree['ressources'], dict):
                ressources = [ {'id' : { 
                                    'length' : len(DataProtocolConverter.encode_id(key, data_model)), 
                                    'fields' : DataProtocolConverter.encode_id(key, data_model)}, 
                                'type'  : DataProtocolConverter.get_type(key, data_model),
                                'value' : int(DataProtocolConverter.get_value(key, value, data_model)) if DataProtocolConverter.get_type(key, data_model) == 'enum' else value}
                                for key, value in tree['ressources'].items()]
            else:
                ressources = [ {'id' : { 
                                    'length' : len(DataProtocolConverter.encode_id(key, data_model)), 
                                    'fields' : DataProtocolConverter.encode_id(key, data_model)}, 
                                'type'  : 'empty',
                                'value' : None}
                                for key in tree['ressources']]
        else:
            length = len(tree['status'])
            ressources = [ {'id' : {
                                    'length' : len(DataProtocolConverter.encode_id(key, data_model)), 
                                    'fields' : DataProtocolConverter.encode_id(key, data_model)}, 
                                'type'  : 'status',
                                'value' : value}
                                for key, value in tree['status'].items()]
        tree_enrich = {'header' : {'command'    : tree['command'],
                                   'direction'  : tree['direction'], 
                                   'sequence'   : tree['sequence'],
                                   'count'      : length},
                       'ressources' : ressources}
        return DataProtocolConverter.FRAME_STRUCT.build(tree_enrich)

if __name__ == '__main__':
    with open('data_model.json') as json_file:
        data_model = json.load(json_file)
    pprint.pprint(DataProtocolConverter.decode(HexString.to_bytes("32AA02010203030202010164000000"), data_model))
    pprint.pprint(DataProtocolConverter.decode(HexString.to_bytes("22AB0201020002020100"), data_model))
    pprint.pprint(DataProtocolConverter.decode(HexString.to_bytes("52AC02010205000202010503"), data_model))
    pprint.pprint(DataProtocolConverter.decode(HexString.to_bytes("431903010301023e99999a040104010102412000000203010302"), data_model))
    lulu = {'command' : 'write',
            'direction' : 'request',
            'sequence' : 25,
            'ressources' : {
                'control.motors.front_left' : 0.3,
                'control.attitude.roll.position' : 10.0,
                'wifi.status' : 'disconnected'
            }}
    print(HexString.from_bytes(DataProtocolConverter.encode(lulu, data_model)))
    lulu = {'command' : 'write',
            'direction' : 'response',
            'sequence' : 26,
            'status' : {
                'control.motors.front_left' : 'success',
                'control.attitude.roll.position' : 'not_found',
                'wifi.status' : 'bad_type'
            }}
    print(HexString.from_bytes(DataProtocolConverter.encode(lulu, data_model)))
    lulu = {'command' : 'read',
            'direction' : 'request',
            'sequence' : 27,
            'ressources' : [
                'control.motors.front_left',
                'control.attitude.roll.position',
                'wifi.status'
            ]}
    print(HexString.from_bytes(DataProtocolConverter.encode(lulu, data_model)))
    lulu = {'command' : 'notify',
            'direction' : 'response',
            'sequence' : 28,
            'ressources' : {
            }}
    print(HexString.from_bytes(DataProtocolConverter.encode(lulu, data_model)))

    class MainWindow(QMainWindow):

        def __init__(self, data_model):
            super().__init__()
            self.set_ui()
            self.data_model = data_model

        def set_ui(self):
            self.setGeometry(600,400,500,250)
            self.setWindowTitle('Protocol Converter')
            self.statusBar().showMessage('Barre de statut')

            self.text_edit=QTextEdit()
            self.text_edit.setCurrentFont(QFont("Menlo Regular"))
            self.text_edit.textChanged.connect(self.on_text_edited)
            self.line_edit=QLineEdit()
            self.line_edit.textChanged.connect(self.on_line_edited)

            self.vbox=QVBoxLayout()
            self.vbox.addStretch(1)
            self.vbox.addWidget(self.text_edit)
            self.vbox.addWidget(self.line_edit)

            self.w = QWidget()
            self.w.setLayout(self.vbox)
            self.setCentralWidget(self.w)

            self.show()

        def on_line_edited(self, text):
            try:
                new_text = json.dumps(DataProtocolConverter.decode(HexString.to_bytes(text), data_model), indent = 4)
                if self.text_edit.toPlainText() != new_text:
                    self.text_edit.setText(new_text)
                    self.line_edit.setStyleSheet("border: 2px solid #11B55F;")
                    self.statusBar().showMessage("")
            except Exception as e:
                self.line_edit.setStyleSheet("border: 2px solid #DD1C1A;")
                self.statusBar().showMessage(str(e))

        def on_text_edited(self):
            try:
                text = self.text_edit.toPlainText()
                new_text = HexString.from_bytes(DataProtocolConverter.encode(json.loads(text), data_model))
                if self.line_edit.text() != new_text:
                    self.line_edit.setText(new_text)
                    self.text_edit.setStyleSheet("border: 2px solid #11B55F;")
                    self.statusBar().showMessage("")
            except Exception as e:
                self.text_edit.setStyleSheet("border: 2px solid #DD1C1A;")
                self.statusBar().showMessage(str(e))

    app = QApplication(sys.argv)

    w = MainWindow(data_model)
    w.line_edit.setText("531a030103010500040104010105010203010502")
    
    sys.exit(app.exec_())