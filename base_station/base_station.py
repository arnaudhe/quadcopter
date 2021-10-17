#! /usr/bin/env python3

import json
import sys
import socket
import time
import argparse
import signal

from datetime import datetime
from PyQt5.QtWidgets import QApplication, QWidget, QLineEdit, QMainWindow, QTextEdit, QVBoxLayout, QHBoxLayout, QSlider, QLabel, QPushButton, QCheckBox, QComboBox, QGroupBox, QScrollArea
from PyQt5.QtGui import QFont
from PyQt5.QtCore import Qt, pyqtSignal, QRect
from threading import Thread
from xbox import XBoxController
from logger import UdpLogger

class FloatSlider(QSlider):

    # create our our signal that we can connect to if necessary
    floatValueChanged = pyqtSignal(float)

    def __init__(self, decimals=3):
        super(FloatSlider, self).__init__(Qt.Horizontal)
        self._multi = 10 ** decimals
        self.valueChanged.connect(self.emitFloatValueChanged)

    def emitFloatValueChanged(self):
        value = float(super(FloatSlider, self).value())/self._multi
        self.floatValueChanged.emit(value)

    def value(self):
        return float(super(FloatSlider, self).value()) / self._multi

    def setMinimum(self, value):
        return super(FloatSlider, self).setMinimum(int(value * self._multi))

    def setMaximum(self, value):
        return super(FloatSlider, self).setMaximum(int(value * self._multi))

    def setSingleStep(self, value):
        return super(FloatSlider, self).setSingleStep(value * self._multi)

    def singleStep(self):
        return float(super(FloatSlider, self).singleStep()) / self._multi

    def setValue(self, value):
        try:
            super(FloatSlider, self).setValue(int(value * self._multi))
        except:
            pass


class DataRessourceWidget(QWidget):

    def __init__(self, value_widget, read_callback, key, permissions):
        super().__init__()
        self.read_callback = read_callback
        self.value_widget = value_widget
        self.key = key
        self.label = QLabel(key.split('.')[-1])
        self.button = QPushButton('read')
        self.button.clicked.connect(self.on_button_clicked)
        if not 'read' in permissions:
            self.button.setEnabled(False)
        if not 'write' in permissions:
            self.value_widget.setEnabled(False)
        self.layout = QHBoxLayout()
        self.layout.addWidget(self.label)
        self.layout.addWidget(value_widget)
        self.layout.addWidget(self.button)
        self.setLayout(self.layout)
        self.read_pending = False
        if 'read' in permissions:
            self.on_button_clicked()

    def on_button_clicked(self):
        status, value = self.read_callback(self.key)
        if status == True:
            self.read_pending = True
            self.set_value(value)
            self.setVisible(False)
            self.setVisible(True)       # Force widget layout update
            self.read_pending = False


class IntegerRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions, unit, minimum=0, maximum=10):
        super().__init__(QSlider(Qt.Horizontal), read_callback, key, permissions)
        self.write_callback = write_callback
        self.unit = unit
        self.value_widget.setMinimum(minimum)
        self.value_widget.setMaximum(maximum)
        self.value_widget.valueChanged.connect(self.on_value_changed)

    def on_value_changed(self, value):
        if self.read_pending == False:
            self.write_callback(self.key, value)

    def set_value(self, value):
        self.value_widget.setValue(value)


class FloatRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions, unit, minimum=0, maximum=10):
        super().__init__(FloatSlider(), read_callback, key, permissions)
        self.write_callback = write_callback
        self.unit = unit
        self.value_widget.setMinimum(minimum)
        self.value_widget.setMaximum(maximum)
        self.value_widget.floatValueChanged.connect(self.on_value_changed)

    def on_value_changed(self, value):
        if self.read_pending == False:
            self.write_callback(self.key, value)

    def set_value(self, value):
        self.value_widget.setValue(value)


class DoubleRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions, unit, minimum=0, maximum=10):
        super().__init__(FloatSlider(), read_callback, key, permissions)
        self.write_callback = write_callback
        self.unit = unit
        self.value_widget.setMinimum(minimum)
        self.value_widget.setMaximum(maximum)
        self.value_widget.floatValueChanged.connect(self.on_value_changed)

    def on_value_changed(self, value):
        if self.read_pending == False:
            self.write_callback(self.key, value)

    def set_value(self, value):
        self.value_widget.setValue(value)


class BoolRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions):
        super().__init__(QCheckBox(), read_callback, key, permissions)
        self.write_callback = write_callback
        self.value_widget.stateChanged.connect(self.on_value_changed)

    def on_value_changed(self, value):
        if self.read_pending == False:
            self.write_callback(self.key, bool(value))

    def set_value(self, value):
        self.value_widget.setChecked(value)


class EnumRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions, values):
        super().__init__(QComboBox(), read_callback, key, permissions)
        self.write_callback = write_callback
        self.values = values
        self.value_widget.addItems(values.keys())
        self.value_widget.currentIndexChanged.connect(self.on_value_changed)

    def on_value_changed(self, index):
        if self.read_pending == False:
            self.write_callback(self.key, self.value_widget.currentText())

    def set_value(self, value):
        self.value_widget.setCurrentIndex(self.value_widget.findText(value))


class MainWindow(QMainWindow):

    def __init__(self, controller):
        super().__init__()
        self.controller = controller
        self.widgets = {}
        self.setup_ui()

    def load_model(self, data_model, container_layout, current_key = ''):
        for key, content in data_model.items():
            new_key = current_key + key
            if isinstance(content, dict):
                if 'type' in content:
                    if content['type'] == 'integer':
                        if 'min' in content:
                            minimum = content['min']
                        else:
                            minimum = 0
                        if 'max' in content:
                            maximum = content['max']
                        else:
                            maximum = 1
                        widget = IntegerRessourceWidget(self.on_write_request, self.on_read_request, new_key, content['permissions'], 
                                                        content['unit'], minimum, maximum)
                    elif content['type'] == 'float':
                        if 'min' in content:
                            minimum = content['min']
                        else:
                            minimum = 0.0
                        if 'max' in content:
                            maximum = content['max']
                        else:
                            maximum = 1.0
                        widget = FloatRessourceWidget(self.on_write_request, self.on_read_request, new_key, content['permissions'], 
                                                    content['unit'], minimum, maximum)
                    elif content['type'] == 'double':
                        if 'min' in content:
                            minimum = content['min']
                        else:
                            minimum = 0.0
                        if 'max' in content:
                            maximum = content['max']
                        else:
                            maximum = 1.0
                        widget = DoubleRessourceWidget(self.on_write_request, self.on_read_request, new_key, content['permissions'], 
                                                    content['unit'], minimum, maximum)
                    elif content['type'] == 'bool':
                        widget = BoolRessourceWidget(self.on_write_request, self.on_read_request, new_key, content['permissions'])
                    elif content['type'] == 'enum':
                        widget = EnumRessourceWidget(self.on_write_request, self.on_read_request, new_key, content['permissions'],
                                                    content['values'])
                    else:
                        raise Exception("Invalid ressource type")
                    self.widgets[new_key] = widget
                    container_layout.addWidget(widget)
                else:
                    vbox = QVBoxLayout()
                    self.load_model(data_model[key], vbox, new_key + '.')
                    widget = QGroupBox(key)
                    widget.setLayout(vbox)
                    container_layout.addWidget(widget)

    def setup_ui(self):
        self.centralWidget = QWidget()
        layout = QVBoxLayout(self.centralWidget)
        self.scrollArea = QScrollArea(self.centralWidget)
        layout.addWidget(self.scrollArea)
        self.scrollAreaWidgetContents = QWidget()
        self.scrollAreaWidgetContents.setGeometry(QRect(0, 0, 970, 3500))
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        layout = QVBoxLayout(self.scrollAreaWidgetContents)     
        self.setCentralWidget(self.centralWidget)
        self.centralWidget.setStyleSheet("QGroupBox { padding-top: 20px; font-size:14px; text-transform: uppercase; border: 1px solid rgb(100, 100, 100); }")
        self.setWindowTitle('Quadcopter base station')
        self.load_model(self.controller.get_data_model(), layout)
        self.setCentralWidget(self.centralWidget)
        self.resize(1000, 800)
        self.show()

    def on_write_request(self, key, value):
        return self.controller.write(key, value)

    def on_read_request(self, key):
        return self.controller.read(key)

class QuadcopterController:

    ROLL_PITCH_TRIM_STEP   = 0.01
    YAW_THROTTLE_TRIM_STEP = 0.02
    STICK_SCALE = 500000

    def __init__(self, data_model):
        self.sequence = 0
        self.data_model = data_model
        self.model_loaded = False
        self.quadcopter_address = ''
        self.quadcopter_name = ''
        self.quadcopter_heartbeat = 5
        print('Waiting for quadcopter announcement...')
        res = self.wait_announcement(blocking = True)
        if not res:
            sys.exit(1)
        print(f'Detected announcement from {self.quadcopter_name} at {self.quadcopter_address}')
        self.running = True
        self.heartbeat_task = Thread(target = self.run_heartbeat)
        self.heartbeat_task.start()
        self.xbox_controller = None

    def attach_xbox_controller(self, xbox_controller):
        self.xbox_controller = xbox_controller
        self.xbox_task = Thread(target = self.run_xbox)
        self.xbox_task.start()

    def get_data_model(self):
        return self.data_model

    def shutdown(self):
        self.running = False
        self.heartbeat_task.join()
        if self.xbox_controller:
            self.xbox_task.join()

    def run_heartbeat(self):
        print('[HEARTBEAT] Started')
        while self.running:
            if self.wait_announcement(blocking = False):
                if self.quadcopter_heartbeat == 0:
                    print(f'[HEARTBEAT] Retrieved signal from {self.quadcopter_name}!')
                self.quadcopter_heartbeat = 5
            else:
                if self.quadcopter_heartbeat > 0:
                    self.quadcopter_heartbeat -= 1
                    if self.quadcopter_heartbeat == 0:
                        print(f'[HEARTBEAT] Lost signal from {self.quadcopter_name}')
        print('[HEARTBEAT] Stopped')

    def run_xbox(self):
        print('[XBOX] Started')
        armed = False
        roll_trim = 0.0
        pitch_trim = 0.0
        yaw_trim = 0.0
        throttle_trim = 0.3
        roll = 0.0
        pitch = 0.0
        yaw = 0.0
        throttle = 0.3
        while self.running:
            state = self.xbox_controller.get_state()
            if state:
                if state['keys']['X']:
                    roll_trim = roll_trim - QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['B']:
                    roll_trim = roll_trim + QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['A']:
                    pitch_trim = pitch_trim - QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['Y']:
                    pitch_trim = pitch_trim + QuadcopterController.ROLL_PITCH_TRIM_STEP
                if state['keys']['left']:
                    yaw_trim = yaw_trim - QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['right']:
                    yaw_trim = yaw_trim + QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['down']:
                    throttle_trim = throttle_trim - QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['up']:
                    throttle_trim = throttle_trim + QuadcopterController.YAW_THROTTLE_TRIM_STEP
                if state['keys']['left_trigger'] and state['keys']['right_trigger']:
                    armed = not armed
                    if armed:
                        self.write('control.attitude.roll.mode', 'position')
                        self.write('control.attitude.pitch.mode', 'position')
                        self.write('control.attitude.yaw.mode', 'speed')
                        self.write('control.mode', 'attitude')
                    else:
                        self.write('control.mode', 'off')
                roll_stick     = state['right_stick']['horizontal'] / QuadcopterController.STICK_SCALE
                pitch_stick    = state['right_stick']['vertical']   / QuadcopterController.STICK_SCALE
                yaw_stick      = state['left_stick']['horizontal']  / QuadcopterController.STICK_SCALE
                throttle_stick = state['left_stick']['vertical']    / QuadcopterController.STICK_SCALE
                if roll_trim + roll_stick != roll:
                    roll = roll_trim + roll_stick
                    self.write('control.attitude.roll.position_target', roll)
                if pitch_trim + pitch_stick != pitch:
                    pitch = pitch_trim + pitch_stick
                    self.write('control.attitude.pitch.position_target', pitch)
                if yaw_trim + yaw_stick != yaw:
                    yaw = yaw_trim + yaw_stick
                    self.write('control.attitude.yaw.speed_target', yaw)
                if throttle_trim + throttle_stick != throttle:
                    throttle = throttle_trim + throttle_stick
                    self.write('control.thurst_offset', throttle)
            time.sleep(0.1)
        print('[XBOX] Stopped')

class QuadcopterUdpController(QuadcopterController):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.udp_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def wait_announcement(self, blocking = False):
        # Bind a new socket on 5001 to detect sent announcements
        udp_detector = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        udp_detector.bind(('0.0.0.0', 5001))
        udp_detector.settimeout(1.1)
        result = False
        while True:
            try:
                data, address = udp_detector.recvfrom(128)
                message = json.loads(data.decode('utf-8'))
                if 'announcement' in message:
                    self.quadcopter_address, _ = address
                    self.quadcopter_name = message["announcement"]
                    result = True
            except KeyboardInterrupt:
                result = False
                break
            except Exception as e:
                pass
            if (blocking == False) or (result == True):
                break
        udp_detector.close()
        return result
    
    def write(self, key, value):
        print(f'[UDP CTRL] write {key} {value}')
        self.sequence = (self.sequence + 1) % 256
        command = {'command' : 'write', 'sequence' : self.sequence, 'direction' : 'request', 'ressources' : {key : value}}
        self.udp_client.sendto(json.dumps(command).encode('utf-8'), (self.quadcopter_address, 5000))
        try:
            data = self.udp_client.recv(1024).decode("utf-8")
        except socket.timeout:
            print(f'[UDP CTRL] Response timeout when writing ressource {key}')
            return
        try:
            response = json.loads(data)
        except:
            print(f'[UDP CTRL] Invalid response')
            return
        if (response['command'] == "write" and response['sequence'] == self.sequence and response['direction'] == 'response'):
            if response['status'][key] == 'success':
                print('[UDP CTRL] Success')
            else:
                print('[UDP CTRL] Write status {}'.format(response['status'][key]))
        else:
            print('[UDP CTRL] Not matching response. Drop it.')

    def read(self, key):
        print(f'[UDP CTRL] read {key}')
        self.sequence = (self.sequence + 1) % 256
        command = {'command' : 'read', 'sequence' : self.sequence, 'direction' : 'request', 'ressources' : [key]}
        self.udp_client.sendto(json.dumps(command).encode('utf-8'), (self.quadcopter_address, 5000))
        self.udp_client.settimeout(1.0)
        try:
            data = self.udp_client.recv(1024).decode("utf-8")
        except socket.timeout:
            print(f'[UDP CTRL] Response timeout when reading ressource {key}')
            return False, None
        try:
            response = json.loads(data)
        except:
            print(f'[UDP CTRL] Invalid response')
            return
        if (response['command'] == "read" and response['sequence'] == self.sequence and response['direction'] == 'response'):
            if response['status'][key] == 'success':
                print(response['ressources'][key])
                return True, response['ressources'][key]
            else:
                print('[UDP CTRL] Read status {}'.format(response['status'][key]))
        else:
            print('[UDP CTRL] Not matching response. Drop it.')

class QuadcopterRadioController(QuadcopterController):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def wait_announcement(self, blocking = False):
        # TODO
        return False

    def write(self, key, value):
        print(f'[RADIO CTRL] write {key} {value}')
        self.sequence = (self.sequence + 1) % 256
        # TODO
        return

    def read(self, key):
        print(f'[RADIO CTRL] read {key}')
        self.sequence = (self.sequence + 1) % 256
        # TODO
        return False, None


if __name__ == '__main__':

    with open('../data_model/data_model.json') as json_file:
        data_model = json.load(json_file)

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('-l', '--logger', action='store_true', help='Enable logging')
    parser.add_argument('-g', '--gui',    action='store_true', help='Enable gui')
    parser.add_argument('-r', '--remote', action='store_true', help='Enable xbox remote controller')
    args = parser.parse_args()

    controller = QuadcopterUdpController(data_model = data_model)

    if args.remote:
        xbox = XBoxController()
        xbox.start()
        controller.attach_xbox_controller(xbox)

    if args.logger:
        logger = UdpLogger()

    if args.gui:
        app = QApplication([])
        w = MainWindow(controller)
        app.exec_()

    if args.remote:
        xbox.stop()

    if args.logger:
        logger.shutdown()

    controller.shutdown()

    sys.exit(0)
