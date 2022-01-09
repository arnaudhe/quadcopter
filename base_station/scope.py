import sys
import json
import numpy as np
import pyqtgraph as pg

class Scope:

    PENS = {
        'RED': (229, 32,  57),
        'ORANGE': (255, 154, 4),
        'BLUE': (0,   162, 232),
        'GREEN': (102, 177, 33)
    }

    CHANNELS_PENS = ['RED', 'BLUE', 'ORANGE', 'GREEN', 'RED', 'BLUE', 'ORANGE', 'GREEN', 'RED', 'BLUE', 'ORANGE', 'GREEN']

    FONT_COLOR             = (200, 200, 200)
    GRID_COLOR             = (150, 150, 150)
    BACKGROUND_LIGHT_COLOR = (50, 50, 50)
    BACKGOURND_DARK_COLOR  = (20, 20, 20)

    def __init__(self, telemetry, dimension, width, height, x_depth, y_min, y_max, channels_desc):
        self.width       = width
        self.height      = height
        self.x_depth     = x_depth
        self.y_min       = y_min
        self.y_max       = y_max
        self.telemetry = telemetry
        self.dimension   = dimension
        self.chan_desc   = channels_desc
        self.plots       = []
        self.fft_plots   = []
        self.generate_ui()

    def start(self):
        self.telemetry.data_ready_signal.connect(self.data_ready_slot)
        self.telemetry.start()

    def stop(self):
        self.telemetry.stop()

    def generate_ui(self):
        self.win = pg.GraphicsLayoutWidget(show=True, title="Scope")
        self.win.resize(self.width, self.height)
        self.graph = self.win.addPlot(title="Scope")
        self.graph.setMouseEnabled(False)
        self.graph.showGrid(True, True)
        self.graph.setXRange(1.0, self.x_depth)
        self.graph.setYRange(-1.0, 1.0)
        self.graph.addLegend(labelTextSize='11pt')
        for i in range(len(self.chan_desc)):
            self.plots.append(self.graph.plot(np.zeros(shape=self.x_depth), pen=Scope.PENS[Scope.CHANNELS_PENS[i]], name=self.chan_desc[i]))

    def update_channel(self, channel_num):
        # x = np.arange(self.x_depth)
        y = self.telemetry.data[:, channel_num]
        # print(y)
        self.plots[channel_num].setData(y)

    def data_ready_slot(self):
        for i in range(len(self.plots)):
            self.update_channel(i)

if __name__ == '__main__':

    from telemetry import TelemetryReaderUdp, TelemetryReaderSerial

    app = pg.mkQApp("Plotting Example")
    pg.setConfigOptions(antialias=True)

    if (len(sys.argv) < 2):
        sys.exit('usage : python {} <config>'.format(sys.argv[0]))

    with open(sys.argv[1], 'r') as config_file:
        config = json.loads(config_file.read())

    data_source_type   = config['source']['type']
    data_source_params = config['source']['params']

    if ('regex' in config['source']):
        data_source_regex = config['source']['regex']
    else:
        data_source_regex = "(.+)"

    channels_len       = len(config['channels'])
    scope_x_depth      = config['scope']['x_depth']
    scope_width        = config['scope']['width']
    scope_height       = config['scope']['height']
    scope_y_min        = config['scope']['y_min']
    scope_y_max        = config['scope']['y_max']
    channels_desc      = ['{} ({})'.format(channel, config['channels'][channel]['unit']) for channel in config['channels']]

    if data_source_type == 'udp':
        telemetry = TelemetryReaderUdp(data_source_params['port'], channels_len, data_source_regex, scope_x_depth)
    elif data_source_type == 'serial':
        telemetry = TelemetryReaderSerial(data_source_params['port'], data_source_params['baudrate'], channels_len, data_source_regex, scope_x_depth)
    else:
        sys.exit("Unknown source type")

    osc = Scope(telemetry, channels_len, scope_width, scope_height, scope_x_depth, scope_y_min, scope_y_max, channels_desc)
    osc.start()
    app.exec_()
    osc.stop()
