
import serial
import socket
import re
import numpy as np
from threading import Thread, Event
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import pyqtSignal, QObject

class TelemetryReader(Thread, QObject):

    DEFAULT_REGEX = r'(.+)'

    OUTPUT_FILE = 'telemetry.csv'

    stop_thread = Event()
    data_ready_signal = pyqtSignal()

    def from_config(config : dict, channels_num : int = 1, data_depth : int = 100):
        regex = config['regex'] if 'regex' in config else TelemetryReader.DEFAULT_REGEX
        if config['type'] == 'udp':
            return TelemetryReaderUdp(config['params']['port'], channels_num=channels_num, regex=regex, depth=data_depth)
        elif config['type'] == 'serial':
            return TelemetryReaderSerial(config['params']['port'], config['params']['baudrate'], channels_num=channels_num, regex=regex, depth=data_depth)
        else:
            raise Exception("Unknown telemetry type")

    def __init__(self, channels_num = 1, regex = DEFAULT_REGEX, depth = 1000):
        Thread.__init__(self)
        QObject.__init__(self)
        self.data_buff_size = depth  # Buffer size
        self.channels_num = channels_num
        self.regex = regex
        self.data = np.zeros((self.data_buff_size, self.channels_num))  # Telemetry buffer
        self.output_file = None

    def output(self, channels):
        print('[TELEMETRY] output data to ' + TelemetryReader.OUTPUT_FILE)
        self.output_file = open(TelemetryReader.OUTPUT_FILE, 'w')
        self.output_file.write(';'.join(channels) + '\n')

    def run(self):
        while not self.stop_thread.isSet() :
            data = self.read()  # Read incoming data
            for line in data.split('\n'):
                if len(line):
                    if re.match(self.regex, line) != None:
                        try:
                            values = re.match(self.regex, line).group(1).split(';')
                            if len(values) == self.channels_num:
                                values = [float(i) for i in values]
                            else:
                                raise Exception("Bad input length")
                            if self.output_file:
                                self.output_file.write(line + '\n')
                        except Exception as e:
                            print(f'Input error : {e}')
                        else:
                            self.data = np.roll(self.data, -1, axis=0)
                            self.data[-1, :] = np.array(values).T
                            self.data_ready_signal.emit()
                    else:
                        print('Data input does not match regex')
        self.close()

    def stop(self):
        self.stop_thread.set()
        self.join()
        if self.output_file:
            self.output_file.close()
            print('[TELEMETRY] ' + TelemetryReader.OUTPUT_FILE + ' closed')

class TelemetryReaderUdp(TelemetryReader):

    def __init__(self, port, **kwargs):
        super(TelemetryReaderUdp, self).__init__(**kwargs)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', port))
        self.sock.settimeout(1.0)
        print('[TELEMETRY] Socket bound on port {}'.format(port))

    def read(self):
        try:
            data = self.sock.recv(1024).decode('utf-8')
        except socket.timeout:
            data = ''
        return data

    def close(self):
        print('[TELEMETRY] Socket closed')
        self.sock.close()

class TelemetryReaderSerial(TelemetryReader):

    def __init__(self, port, baudrate = 115200, **kwargs):
        super(TelemetryReaderSerial, self).__init__(**kwargs)
        self.ser = serial.Serial(port, baudrate, timeout=1.0)  # open serial port
        print('[TELEMETRY]  Serial port {} opened'.format(port))

    def read(self):
        return self.ser.readline()

    def close(self):
        print('[TELEMETRY] Serial closed')
        self.ser.close()

if __name__ == '__main__':

    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('channels_num', help="number of data received at a time", type=int)
    parser.add_argument("medium", help="telemetry medium", type=str, choices=['udp', 'serial'])
    parser.add_argument("port", help="port to use (socket for udp, serial port)", type=int)
    parser.add_argument("-b", "--baudrate", type=int, default=115200)
    args = parser.parse_args()

    global reader

    def data_ready_event():
        print(f'Data received : {reader.data[0]}')

    app = QApplication([])
    if args.medium == 'udp':
        reader = TelemetryReaderUdp(args.port, channels_num=args.channels_num, depth=1)
    else:
        reader = TelemetryReaderSerial(args.port, args.baudrate, channels_num=args.channels_num, depth=1)

    reader.data_ready_signal.connect(data_ready_event)

    reader.start()
    app.exec_()
    reader.stop()
