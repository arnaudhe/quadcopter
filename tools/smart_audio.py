from construct import Struct, Enum, Int8ub, Const, RawCopy, Checksum, this
from serial import Serial, STOPBITS_TWO

class crc8(object):

    digest_size = 1
    block_size = 1

    _table = [ 0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
               0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
               0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
               0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
               0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
               0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
               0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
               0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
               0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
               0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
               0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
               0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
               0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
               0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
               0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
               0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9 ]

    def __init__(self, initial_string=b'', initial_start=0x00):
        """Create a new crc8 hash instance."""
        self._sum = initial_start
        self._update(initial_string)

    def update(self, bytes_):
        """Update the hash object with the string arg.

        Repeated calls are equivalent to a single call with the concatenation
        of all the arguments: m.update(a); m.update(b) is equivalent
        to m.update(a+b).
        """
        self._update(bytes_)

    def digest(self):
        """Return the digest of the bytes passed to the update() method so far.

        This is a string of digest_size bytes which may contain non-ASCII
        characters, including null bytes.
        """
        return self._digest()

    def hexdigest(self):
        """Return digest() as hexadecimal string.

        Like digest() except the digest is returned as a string of double
        length, containing only hexadecimal digits. This may be used to
        exchange the value safely in email or other non-binary environments.
        """
        return hex(self._sum)[2:].zfill(2)

    def _update(self, bytes_):
        if isinstance(bytes_, str):
            raise TypeError("Unicode-objects must be encoded before"\
                            " hashing")
        elif not isinstance(bytes_, (bytes, bytearray)):
            raise TypeError("object supporting the buffer API required")
        table = self._table
        _sum = self._sum
        for byte in bytes_:
            _sum = table[_sum ^ byte]
        self._sum = _sum

    def _digest(self):
        return bytes([self._sum])

    def copy(self):
        """Return a copy ("clone") of the hash object.
        
        This can be used to efficiently compute the digests of strings that
        share a common initial substring.
        """
        crc = crc8()
        crc._sum = self._sum
        return crc


class SmartAudioFrame(Struct):
    """
    SmartAudioFrame is a specification of Frame
    """

    @staticmethod
    def compute_crc(data):
        return crc8(bytes([0xAA, 0x55]) + data).digest()[0]

    @staticmethod
    def generic_command(command, params):
        return SmartAudioFrame().build(dict(body=dict(value=dict(command=command, params=params, length=len(params)))))

    @staticmethod
    def set_power(power):
        return SmartAudioFrame.generic_command('SET_POWER', [power])

    def __init__(self):
        super().__init__(
            'sync' / Const(bytes([0xAA, 0x55])),
            "body" / RawCopy(Struct(
                'command' / Enum(Int8ub, GET_SETTINGS=0x3, SET_POWER=0x5, SET_CHANNEL=0x7, SET_FREQUENCY=0x9, SET_OPERATION_MODE=0xB),
                'length'  / Int8ub,
                'params'  / Int8ub[this.length],
            )),
            'crc' / Checksum(Int8ub, lambda data: SmartAudioFrame.compute_crc(data), this.body.data)
        )

class SmartAudio:

    def __init__(self, port):
        self._ser = Serial(port=port, baudrate=4800, stopbits=STOPBITS_TWO)
        print('SmartAudio port opened')

    def send(self, frame: SmartAudioFrame):
        print(f'Send {frame.hex()}')
        self._ser.write(frame)

    def close(self):
        self._ser.close()
        print('SmartAudio port closed')

if __name__ == '__main__':

    set_power_frame = SmartAudioFrame.set_power(0)
    print(set_power_frame.hex())

    smart_audio = SmartAudio('/dev/ttyUSB0')
    smart_audio.send(set_power_frame)
    smart_audio.close()