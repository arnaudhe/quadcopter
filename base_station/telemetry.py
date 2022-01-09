
from PyQt5.QtWidgets import QApplication
import serial
import socket
import numpy as np
import re
from threading import Thread, Event
from PyQt5.QtCore import pyqtSignal, QObject

class TelemetryReader(Thread, QObject):

    stop_thread = Event()
    data_ready_signal = pyqtSignal()

    def __init__(self, dimension = 1, regex = r'(.+)', depth = 1000):
        Thread.__init__(self)
        QObject.__init__(self)
        self.data_buff_size = depth     # Buffer size
        self.dimension = dimension
        self.regex = regex
        self.data = np.zeros((self.data_buff_size, self.dimension))   # Telemetry buffer

    def run(self):
        while not self.stop_thread.isSet() :
            data = self.read()                          # Read incoming data
            if len(data):
                if re.match(self.regex, data) != None:
                    try:
                        values = re.match(self.regex, data).group(1).split(';')
                        if (len(values) == self.dimension):
                            values = [float(i) for i in values]
                        else:
                            raise Exception("Bad input length")
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

class TelemetryReaderUdp(TelemetryReader):

    def __init__(self, port, **kwargs):
        super(TelemetryReaderUdp, self).__init__(**kwargs)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', port))
        self.sock.settimeout(1.0)
        print('Socket bound on port {}'.format(port))

    def read(self):
        try:
            data = self.sock.recv(1024).decode('utf-8')
        except socket.timeout:
            data = ''
        return data

    def close(self):
        print('Socket closed')
        self.sock.close()

class TelemetryReaderSerial(TelemetryReader):

    def __init__(self, port, baudrate = 115200, **kwargs):
        super(TelemetryReaderSerial, self).__init__(**kwargs)
        self.ser = serial.Serial(port, baudrate, timeout=1.0)  # open serial port
        print('Serial port {} opened'.format(port))

    def read(self):
        return self.ser.readline()

    def close(self):
        print('Serial closed')
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
        reader = TelemetryReaderUdp(args.port, dimension=args.channels_num, depth=1)
    else:
        reader = TelemetryReaderSerial(args.port, args.baudrate, dimension=args.channels_num, depth=1)

    reader.data_ready_signal.connect(data_ready_event)

    reader.start()
    app.exec_()
    reader.stop()
