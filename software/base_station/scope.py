import json
import numpy as np
import pyqtgraph as pg
import os

from telemetry import TelemetryReader

class ScopeConfig:

    DEFAULT_REGEX = r"(.+)"

    def __init__(self, config_file):
        if not os.path.isfile(config_file):
            raise Exception('Not existing config file')
        try:
            with open(config_file, 'r') as config_file:
                config = json.loads(config_file.read())
            self.telemetry_config = config['source']
            self.data_depth       = config['scope']['x_depth']
            self.window_size      = (config['scope']['width'], config['scope']['height'])
            self.y_range          = (config['scope']['y_min'], config['scope']['y_max'])
            self.period           = config['period'] if 'period' in config else 1.0
            self.channels         = list(config['channels'].keys())
            self.fft_channels     = [channel for channel in config['channels'] if ('fft' in config['channels'][channel]) and (config['channels'][channel]['fft'])]
        except:
            raise Exception('Invalid config file')

    @property
    def channels_num(self):
        return len(self.channels)


class Scope:

    PENS = {
        'RED': (229, 32, 57),
        'ORANGE': (255, 154, 4),
        'BLUE': (0, 162, 232),
        'GREEN': (102, 177, 33),
        'DARK_BLUE': (0, 0, 255),
        'PURPLE': (0xa3, 0x2c, 0xc4),
        'BROWN': (0x65, 0x2a, 0x0e),
        'YELLOW': (255, 221, 0)
    }

    CHANNELS_PENS = ['RED', 'BLUE', 'ORANGE', 'GREEN', 'YELLOW', 'PURPLE', 'BROWN', 'DARK_BLUE', 'RED', 'BLUE', 'ORANGE', 'GREEN']

    FONT_COLOR             = (200, 200, 200)
    GRID_COLOR             = (150, 150, 150)
    BACKGROUND_LIGHT_COLOR = (50, 50, 50)
    BACKGOURND_DARK_COLOR  = (20, 20, 20)

    @staticmethod
    def from_config(config_file, output_file = False):
        config = ScopeConfig(config_file)
        telemetry = TelemetryReader.from_config(config.telemetry_config, config.channels_num, config.data_depth)
        if output_file:
            telemetry.output(config.channels)
        return Scope(telemetry, config.data_depth, config.window_size, config.y_range, config.channels, config.fft_channels, config.period)

    def __init__(self, telemetry, data_depth, window_size, y_range, channels, fft_channels = [], period = 1.0):
        self.data_depth   = data_depth
        self.telemetry    = telemetry
        self.channels     = channels
        self.fft_channels = fft_channels
        self.time_plots   = []
        self.fft_plots    = []
        self.period       = period
        self.generate_time_ui(window_size, y_range)
        if len(self.fft_channels):
            self.generate_fft_ui(window_size)

    def start(self):
        self.telemetry.data_ready_signal.connect(self.data_ready_slot)
        self.telemetry.start()

    def stop(self):
        self.telemetry.stop()

    def generate_time_ui(self, window_size, y_range):
        self.win = pg.GraphicsLayoutWidget(show=True, title="Scope")
        self.win.resize(*window_size)
        self.graph = self.win.addPlot(title="Scope")
        self.graph.setMouseEnabled(False)
        self.graph.showGrid(True, True)
        self.graph.setXRange(1.0, self.data_depth)
        self.graph.setYRange(*y_range)
        self.graph.addLegend(labelTextSize='11pt')
        for i in range(len(self.channels)):
            self.time_plots.append(self.graph.plot(np.zeros(shape=self.data_depth), pen=Scope.PENS[Scope.CHANNELS_PENS[i]], name=self.channels[i]))

    def generate_fft_ui(self, window_size):
        self.fft = pg.GraphicsLayoutWidget(show=True, title="FFT")
        self.fft.resize(*window_size)
        self.fft.setWindowTitle('FFT')
        self.fft_graph = self.fft.addPlot(title="FFT")
        self.fft_graph.showGrid(True, True)
        self.fft_graph.addLegend(labelTextSize='11pt')
        for i in range(len(self.channels)):
            if self.channels[i] in self.fft_channels:
                self.fft_plots.append(self.fft_graph.plot(np.zeros(shape=self.data_depth), pen=Scope.PENS[Scope.CHANNELS_PENS[i]], name=self.channels[i]))

    def update_channel(self, channel_num):
        # x = np.arange(self.data_depth)
        y = self.telemetry.data[:, channel_num]
        # print(y)
        self.time_plots[channel_num].setData(y)

    def update_fft(self, channel_num, fft_index):
        rate = (1 / self.period)  # sampling rate
        z = 20 * np.log10(np.abs(np.fft.rfft(self.telemetry.data[:, channel_num]))) #rfft trims imag and leaves real values
        f = np.linspace(0, rate/2, len(z))
        self.fft_plots[fft_index].setData(f, z)

    def data_ready_slot(self):
        fft_index = 0
        for i in range(len(self.time_plots)):
            self.update_channel(i)
            if self.channels[i] in self.fft_channels:
                self.update_fft(i, fft_index)
                fft_index = fft_index + 1


if __name__ == '__main__':

    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('config_file', help="number of data received at a time", type=str)
    args = parser.parse_args()

    app = pg.mkQApp("Scope Example")
    pg.setConfigOptions(antialias=True)

    scope = Scope.from_config(args.config_file)
    scope.start()
    app.exec_()
    scope.stop()