import time
from queue import Queue
from threading import Thread, Lock

from construct import *
from pyftdi.gpio import *
from pyftdi.spi import *
from pyftdi.usbtools import *

from broker import Broker, ChannelListener
from worker import PeriodicWorker

from crc16 import crc16xmodem

class RadioConfig:

    CONFIG_VALUES = [ (0x00, 0x10), (0x01, 0xA4), (0x02, 0x09), (0x03, 0x0F), (0x04, 0x13),
                      (0x05, 0xC4), (0x06, 0x77), (0x07, 0x2F), (0x08, 0x10), (0x09, 0x77),
                      (0x0A, 0x2F), (0x0B, 0x19), (0x0C, 0xC4), (0x0D, 0x1C), (0x0E, 0x09),
                      (0x0F, 0x00), (0x10, 0xA3), (0x11, 0x38), (0x12, 0x38), (0x13, 0x0C),
                      (0x14, 0x00), (0x15, 0x00), (0x16, 0x66), (0x17, 0x66), (0x18, 0x59),
                      (0x19, 0x59), (0x1A, 0x70), (0x1B, 0x3C), (0x1C, 0xC0), (0x1D, 0x00),
                      (0x1E, 0xE0), (0x1F, 0x80) ]

    REGISTERS_NB = 32

    MC1_REG = BitStruct (
        "mode"       / Enum(BitsInteger(3), sleep=0, standby=1, fs=2, rx=3, tx=4),
        "freq_band"  / Enum(BitsInteger(3), f300_330=0, f320_350=1, f350_390=2, f390_430=3, f430_470=4, f470_510=5),
        "sub_band"   / Enum(BitsInteger(2), first_quarter=0, second_quarter=1, third_quarter=2, fourth_quarter=3)
    )

    MC2_REG = BitStruct (
        "data_mode"          / Enum(BitsInteger(2), continuous=0, buffered=1, packet=2),
        "modulation"         / Enum(BitsInteger(2), reset=0, ook=1, fsk=2, reserved=3),
        "ook_threshold_type" / Enum(BitsInteger(2), fixed=0, peak=1, average=2, reserved=3),
        "if_gain"            / BitsInteger(2)
    )

    MC6_REG = BitStruct (
        "pa_ramp"      / BitsInteger(2),
        "low_power_rx" / Flag,
        Padding(2),
        "trim_band"    / BitsInteger(2),
        "rf_frequency" / BitsInteger(1)
    )

    FIFO_REG = BitStruct (
        "fifo_size" / Enum(BitsInteger(2), fifo_16=0, fifo_32=1, fifo_48=2, fifo_64=3),
        "fifo_threshold" / BitsInteger(6)
    )

    IRQ_PARAM_1_REG = BitStruct (
        "rx_irq_0"         / Enum(BitsInteger(2), payload_ready=0, write_byte=1, n_fifo_empty=2, sync_match=3),
        "rx_irq_1"         / Enum(BitsInteger(2), crc_ok = 0, fifo_full=1, rssi=2, fifo_threshold=3),
        "tx_start_irq_0"   / Enum(BitsInteger(1), start_over_threshold=0, start_fifo_not_empty=1),
        "tx_irq_1"         / Enum(BitsInteger(1), tx_done=1, fifo_full=0),
        "fifo_full"        / Flag,
        "not_fifo_empty"   / Flag
    )

    IRQ_PARAM_2_REG = BitStruct (
        "fifo_fill_method" / Enum(Flag, auto=False, manual=True),
        "fifo_fill"        / Flag,
        "tx_done"          / Flag,
        "fifo_overrun_clr" / Flag,
        "reserved"         / BitsInteger(1),
        "rssi"             / Flag,
        "pll_locked"       / Flag,
        "pll_locked_en"    / Flag
    )

    RX_PARAM_1_REG = BitStruct (
        "passive_filter"     / BitsInteger(4),
        "butterworth_filter" / BitsInteger(4)
    )

    RX_PARAM_2_REG = BitStruct (
        "polyfilt_filter" / BitsInteger(4),
        "reserved"        / BitsInteger(4)
    )

    RX_PARAM_3_REG = BitStruct (
        "polyfilt_on"    / Flag,
        "bitsync_off"    / Flag,
        "sync_on"        / Flag,
        "sync_size"      / BitsInteger(2),
        "sync_tolerance" / BitsInteger(2),
        Padding(1)
    )

    RX_PARAM_6_REG = BitStruct (
        "ook_threshold_step"            / BitsInteger(3),
        "ook_threshold_dec_period"      / BitsInteger(3),
        "ook_average_threshold_cut_off" / BitsInteger(2)
    )

    TX_PARAM_REG = BitStruct (
        "interpolation_filter" / BitsInteger(4),
        "p_out"                / Enum(BitsInteger(3), dB_13=0, dB_10=1, dB_7=2, dB_4=3, dB_1=4, dB_minus_1=5, dB_minus_3=6, dB_minus_7=7),
        "tx_zero_if"           / Flag
    )

    OSC_PARAM_REG = BitStruct (
        "clk_out_on"   / Flag,
        "clk_out_freq" / BitsInteger(5),
        Padding(2)
    )

    PKT_PARAM_1_REG = BitStruct (
        "manchester_on"  / Flag,
        "payload_length" / BitsInteger(7)
    )

    PKT_PARAM_3_REG = BitStruct (
        "format"         / Enum(BitsInteger(1), fixed_length=0, variable_length=1),
        "preamble_size"  / Enum(BitsInteger(2), bytes_1=0, bytes_2=1, bytes_3=2, bytes_4=3),
        "whitening_on"   / Flag,
        "crc_on"         / Flag,
        "address_filter" / Enum(BitsInteger(2), off=0, on=1, on_00=2, on_00_ff=3),
        "crc_status"     / Flag
    )

    PKT_PARAM_4_REG = BitStruct (
        "crc_autoclear"       / Flag,
        "fifo_standby_access" / Enum(Flag, write=False, read=True),
        Padding(6)
    )

    CONFIG_DICT = {
        'mc1'              : MC1_REG,         # 0
        'mc2'              : MC2_REG,         # 1
        'f_dev'            : Byte,            # 2
        'bitrate_c'        : Byte,            # 3
        'bitrate_d'        : Byte,            # 4
        'mc6'              : MC6_REG,         # 5
        'r'                : Byte,            # 6
        'p'                : Byte,            # 7
        's'                : Byte,            # 8
        'r2'               : Byte,            # 9
        'p2'               : Byte,            # 10
        's2'               : Byte,            # 11
        'fifo'             : FIFO_REG,        # 12
        'irq_param_1'      : IRQ_PARAM_1_REG, # 13
        'irq_param_2'      : IRQ_PARAM_2_REG, # 14
        'rssi_irq_thresh'  : Byte,            # 15
        'rx_param_1'       : RX_PARAM_1_REG,  # 16
        'rx_param_2'       : RX_PARAM_2_REG,  # 17
        'rx_param_3'       : RX_PARAM_3_REG,  # 18
        'ook_threshold'    : Byte,            # 19
        'rssi_value'       : Byte,            # 20
        'rx_param_6'       : RX_PARAM_6_REG,  # 21
        'sync_1'           : Byte,            # 22
        'sync_2'           : Byte,            # 23
        'sync_3'           : Byte,            # 24
        'sync_4'           : Byte,            # 25
        'tx_param'         : TX_PARAM_REG,    # 26
        'osc_param'        : OSC_PARAM_REG,   # 27
        'pkt_param_1'      : PKT_PARAM_1_REG, # 28
        'node_addr'        : Byte,            # 29
        'pkt_param3'       : PKT_PARAM_3_REG, # 30
        'pkt_param4'       : PKT_PARAM_4_REG  # 31
    }

    CONFIG_STRUCT = Struct(**CONFIG_DICT)

    def __init__(self):
        self.registers = [0] * RadioConfig.REGISTERS_NB
        for address, value in RadioConfig.CONFIG_VALUES:
            self.registers[address] = value
        self.config = RadioConfig.CONFIG_STRUCT.parse(Array(32, Byte).build(self.registers))

    def get_register_address(self, register_name):
        i = 0
        for register in RadioConfig.CONFIG_DICT.keys():
            if register == register_name:
                return i
            i = i + 1
        raise Exception("register {} is invalid", register_name)

    def get_register(self, register):
        if type(register) is int:
            address = register
        else:
            address = self.get_register_address(register)
        return self.registers[address]

    def get_field(self, register, field):
        return self.config[register][field]

    def set_register(self, register, value):
        if type(register) is int:
            address = register
        else:
            address = self.get_register_address(register)
        self.registers[address] = value
        self.config = RadioConfig.CONFIG_STRUCT.parse(Array(32, Byte).build(self.registers))

    def set_field(self, register, field, value):
        self.config[register][field] = value
        self.registers = Array(32, Byte).parse(RadioConfig.CONFIG_STRUCT.build(self.config))
        self.config = RadioConfig.CONFIG_STRUCT.parse(Array(32, Byte).build(self.registers))

    def display(self):
        print(self.config)

class Radio(Thread):

    SPI_CONFIG_FRAME = BitStruct (
        'start'   / BitsInteger(1),
        'rw'      / Enum(Flag, w=0, r=1),
        'address' / BitsInteger(5),
        'stop'    / BitsInteger(1)
    )

    def reset(self):
        self.gpio.set_direction(0x20, 0x20)
        self.gpio.write(0x20)
        time.sleep(0.05)
        self.gpio.write(0x00)
        time.sleep(0.05)
        self.gpio.set_direction(0x20, 0x00)

    def __init__(self, index):
        Thread.__init__(self)
        self.fxosc = 12.8e6
        self.spi = SpiController(cs_count=2, turbo=True)
        print('Open interface 1...')
        device = UsbTools.get_device(UsbDeviceDescriptor(vid=0x403, pid=0x6010, bus=None, address=None, sn=None, index=index, description='SX1212SKA'))
        self.spi.configure('ftdi://ftdi:ft2232d:{}/1'.format(device.serial_number), turbo=True, debug=False)
        print('Done !')
        self.spi_config = self.spi.get_port(cs=0, freq=1E6, mode=0)
        self.spi_data   = self.spi.get_port(cs=1, freq=1E6, mode=0)
        self.spi_config.flush()
        self.spi_data.flush()
        self.gpio = GpioController()
        print('Open interface 2...')
        self.gpio.configure('ftdi://ftdi:ft2232d:{}/2'.format(device.serial_number), 0x00)
        self.gpio.set_frequency(1000)
        print('Done !')
        print('Reset Radio')
        self.reset()
        self.config = RadioConfig()
        for i in range(0, RadioConfig.REGISTERS_NB):
            if i != 20:
                self.write_register(i, self.config.get_register(i))
                self.read_register(i)
        self.goto_standby()
        self.rx_running = False
        self.last_rssi = -130
        self.queue = Queue()

    def close(self):
        self.spi.terminate()
        self.gpio.close()

    def read_gpio(self, line):
        value = self.gpio.read_port(peek=True)
        return ((value & (1 << line)) == (1 << line))

    def read_irq0(self):
        return self.read_gpio(0)

    def read_irq1(self):
        return self.read_gpio(1)

    def write(self, address, value):
        msb = ord(Radio.SPI_CONFIG_FRAME.build({'start':0, 'rw':'w', 'address':address, 'stop':0}))
        self.spi_config.exchange([msb, value])

    def read(self, address):
        msb = ord(Radio.SPI_CONFIG_FRAME.build({'start':0, 'rw':'r', 'address':address, 'stop':0}))
        read = self.spi_config.exchange([msb], 1)
        return read[0]

    def write_register(self, register, value):
        if type(register) is int:
            address = register
        else:
            address = self.config.get_register_address(register)
        self.config.set_register(register, value)
        self.write(address, self.config.get_register(register))

    def read_register(self, register):
        if type(register) is int:
            address = register
        else:
            address = self.config.get_register_address(register)
        value = self.read(address)
        self.config.set_register(register, value)
        return value

    def read_field(self, register, field):
        if type(register) is int:
            address = register
        else:
            address = self.config.get_register_address(register)
        value = self.read(address)
        self.config.set_register(register, value)
        return self.config.get_field(register, field)

    def write_field(self, register, field, value):
        if type(register) is int:
            address = register
        else:
            address = self.config.get_register_address(register)
        old_value = self.read(address)
        self.config.set_register(register, old_value)
        self.config.set_field(register, field, value)
        self.write(address, self.config.get_register(register))

    def get_fxosc(self):
        return self.fxosc

    def get_bitrate(self):
        return self.fxosc / (2 * (self.read_register('bitrate_c') + 1) * (self.read_register('bitrate_d') + 1))

    def get_frequency(self):
        reg_r = self.read_register('r')
        reg_p = self.read_register('p')
        reg_s = self.read_register('s')
        return int((9.0 * self.fxosc * (75.0 * (reg_p + 1) + reg_s)) / (8.0 * (reg_r + 1)))

    def get_frequency_deviation(self):
        return self.fxosc / (32.0 * (1 + self.read_register('f_dev')))

    def goto_sleep(self):
        self.write_field('mc1', 'mode', 'sleep')

    def goto_standby(self):
        self.write_field('mc1', 'mode', 'standby')

    def goto_fs(self):
        self.write_field('mc1', 'mode', 'fs')

    def goto_tx(self):
        self.write_field('mc1', 'mode', 'tx')

    def goto_rx(self):
        self.write_field('mc1', 'mode', 'rx')

    def get_mode(self):
        return self.read_field('mc1', 'mode')

    def read_rssi(self):
        return (int)(self.read_register('rssi_value') * 0.6 - 124)

    def start_rx(self):
        if not self.rx_running:
            Thread.__init__(self)
            time.sleep(0.005)
            self.goto_fs()
            time.sleep(0.005)
            self.goto_rx()
            self.rx_running = True
            self.start()

    def stop_rx(self):
        if self.rx_running:
            self.rx_running = False
            self.join()
            self.goto_standby()

    def transmit(self, frame):
        if self.rx_running:
            need_to_start_rx = True
            self.stop_rx()
            self.join()
        else:
            need_to_start_rx = False
        self.write_field('pkt_param4', 'fifo_standby_access', 'write')
        for b in frame:
            self.spi_data.exchange([b], readlen=0)
        self.goto_fs()
        time.sleep(0.001)
        self.goto_tx()
        while self.read_irq1() == 0:
            time.sleep(0.001)
        self.goto_standby()
        if need_to_start_rx:
            self.start_rx()

    def run(self):
        rssi = -130
        while self.rx_running:
            if self.read_irq0():
                length = int(self.spi_data.exchange([], readlen=1)[0])
                if (length > 0) and (length < 64):
                    frame = bytes([self.spi_data.exchange([], readlen=1)[0] for _ in range(length)])
                    self.queue.put((length, frame, rssi))
            rssi = self.read_rssi()
            time.sleep(0.003)

    def received_frame_pending(self):
        return not self.queue.empty()

    def receive(self):
        if self.queue.empty():
            return None
        else:
            return self.queue.get()

class RadioChannelListener(ChannelListener):
    """
    This class implements a channel listener for a given communication channel
    Radio listener does not manage internally data receiving, as channel multiplexing
    is performed by radio frame serialization
    It thus expects data received for a specific listener, ie channel, to be externally
    enqueue in its reception queue
    """

    def __init__(self, channel: int, name: str, address_filtering: str = None):
        super().__init__(address_filtering)
        self._channel = channel
        self._name    = name

class RadioFrame(Adapter):

    STRUCT = RawCopy(Struct(
        'fields' / RawCopy(Struct(
            'length'  / Byte,
            'inner'   / BitStruct(
                'direction' / Enum(BitsInteger(1), uplink=1, downlink=0),
                'address'   / BitsInteger(7),
            ),
            'channel' / Enum(Byte, **Broker.CHANNELS),
            'payload' / Bytes(this.length - 4))),
        'crc' / Checksum(Int16ub, lambda data: crc16xmodem(data), this.fields.data)
    ))

    def __init__(self):
        super().__init__(RadioFrame.STRUCT)

    def _encode(self, obj, context, path):
        """Overload of construct.Adapter "_encode" method"""
        return  dict(value=dict(fields=dict(value=dict(length=len(obj['data']) + 4,
                                                       inner=dict(direction=obj['direction'],
                                                                  address=obj['address']),
                                                       channel=obj['channel'],
                                                       payload=obj['data']))))

    def _decode(self, obj, context, path):
        """Overload of construct.Adapter "_decode" method"""
        return dict(direction=str(obj.value.fields.value.inner.direction), channel=str(obj.value.fields.value.channel),
                    address=obj.value.fields.value.inner.address, data=obj.value.fields.value.payload)

class RadioBroker(Broker, PeriodicWorker):
    """
    The class is the specification of broker for radio-based communication
    As channels multiplexing is done with radio frame serialization, the broker
    handles global radio reception, demux and dipatching to registered listeners queue.
    It also handles data sending to radio medium
    """

    def __init__(self, index = 0):
        Broker.__init__(self)
        PeriodicWorker.__init__(self, 0.05)
        self._radio = Radio(index)
        self._radio_lock = Lock()
        self._link_quality = 0

    def start(self):
        """Broker start routine"""
        if not self.running():
            self._radio.start_rx()
            PeriodicWorker.start(self)

    def _rssi_to_link_quality(self, rssi):
        """Convert and stores current link quality from rssi"""
        min_rssi = -100
        max_rssi = -41
        if rssi <= min_rssi:
            self._link_quality = 0
        elif rssi >= max_rssi:
            self._link_quality = 100
        else:
            self._link_quality = int((100 / (max_rssi - min_rssi)) * rssi + 170)

    def _run(self):
        """Radio listening main loop"""
        self._radio_lock.acquire()
        if self._radio.received_frame_pending():
            length, frame_bytes, rssi = self._radio.receive()
            try:
                frame = RadioFrame().parse(frame_bytes)
            except ConstructError:
                print('Invalid frame')
            if frame['direction'] == 'downlink' and self.is_registered(frame['channel']):
                self._rssi_to_link_quality(rssi)
                self._listeners[frame['channel']].enqueue(frame['address'], frame['data'])
        self._radio_lock.release()

    def _send_to(self, address: int, channel: str, data: bytes):
        """Format and send a frame to the requested destination address"""
        frame_bytes = RadioFrame().build(dict(direction='uplink', address=address, channel=channel, data=data))
        self._radio_lock.acquire()
        self._radio.transmit(frame_bytes)
        self._radio_lock.release()

    def stop(self):
        """Broker stop routine"""
        if self.running():
            self._radio.stop_rx()
            PeriodicWorker.stop(self)

    def get_link_quality(self):
        """Returns current link quality"""
        return self._link_quality

if __name__ == '__main__':
    radio = RadioBroker(0)
    radio.set_quadcopter_address(31)
    for i in range(3):
        print('transmit')
        radio.Send('heartbeat', b'Hello World')
        time.sleep(1.0)
    radio.start_rx()
    print("wait 10s")
    for _ in range(10):
        time.sleep(1.0)
    radio.stop()
