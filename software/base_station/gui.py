#! /usr/bin/env python3

from PyQt5.QtWidgets import QWidget, QLineEdit, QMainWindow, QVBoxLayout, QHBoxLayout, QSlider, QLabel, QPushButton, QCheckBox, QComboBox, QGroupBox, QScrollArea
from PyQt5.QtCore import Qt, pyqtSignal, QRect


HIDDEN_RESOURCES = [
    "control.position",
    "control.attitude.yaw.speed",
    "control.attitude.yaw.position",
    "control.attitude.height.position",
    "sensors"
]

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

    def on_button_clicked(self):
        status, value = self.read_callback(self.key)
        if status == True:
            self.read_pending = True
            self.set_value(value)
            self.read_pending = False


class IntegerRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions, unit, minimum=0, maximum=10):
        super().__init__(QSlider(Qt.Horizontal), read_callback, key, permissions)
        self.write_callback = write_callback
        self.unit = unit
        self.value_widget.setMinimum(minimum)
        self.value_widget.setMaximum(maximum)
        self.value_widget.valueChanged.connect(self.on_value_changed)
        if 'read' in permissions:
            self.on_button_clicked()

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
        if 'read' in permissions:
            self.on_button_clicked()

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
        if 'read' in permissions:
            self.on_button_clicked()

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
        if 'read' in permissions:
            self.on_button_clicked()

    def on_value_changed(self, value):
        if self.read_pending == False:
            self.write_callback(self.key, bool(value))

    def set_value(self, value):
        self.value_widget.setChecked(value)


class StringRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions):
        super().__init__(QLineEdit(), read_callback, key, permissions)
        self.write_callback = write_callback
        self.value_widget.textChanged.connect(self.on_value_changed)
        if 'read' in permissions:
            self.on_button_clicked()

    def on_value_changed(self, value):
        if self.read_pending == False:
            self.write_callback(self.key, str(value))

    def set_value(self, value):
        self.value_widget.setText(value)


class EnumRessourceWidget(DataRessourceWidget):

    def __init__(self, write_callback, read_callback, key, permissions, values):
        super().__init__(QComboBox(), read_callback, key, permissions)
        self.write_callback = write_callback
        self.values = values
        self.value_widget.addItems(values.keys())
        self.value_widget.currentIndexChanged.connect(self.on_value_changed)
        if 'read' in permissions:
            self.on_button_clicked()

    def on_value_changed(self, index):
        if self.read_pending == False:
            self.write_callback(self.key, self.value_widget.currentText())

    def set_value(self, value):
        self.value_widget.setCurrentIndex(self.value_widget.findText(value))


class BaseStationGui(QMainWindow):

    def __init__(self, controller, data_model):
        super().__init__()
        self.controller = controller
        self.widgets = {}
        self.setup_ui(data_model)

    def load_model(self, data_model, container_layout, current_key = ''):
        for key, content in data_model.items():
            new_key = current_key + key
            if new_key not in HIDDEN_RESOURCES:
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
                        elif content['type'] == 'string':
                            widget = StringRessourceWidget(self.on_write_request, self.on_read_request, new_key, content['permissions'])
                        else:
                            raise Exception(f"Invalid ressource type {content['type']}")
                        self.widgets[new_key] = widget
                        container_layout.addWidget(widget)
                    else:
                        vbox = QVBoxLayout()
                        self.load_model(data_model[key], vbox, new_key + '.')
                        widget = QGroupBox(key)
                        widget.setLayout(vbox)
                        container_layout.addWidget(widget)

    def setup_ui(self, data_model):
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
        self.load_model(data_model, layout)
        self.setCentralWidget(self.centralWidget)
        self.resize(1000, 800)
        self.show()

    def on_write_request(self, key, value):
        return self.controller.write(key, value)

    def on_read_request(self, key):
        return self.controller.read(key)
