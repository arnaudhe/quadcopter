#include <hal/radio.hpp>
#include <utils/crc16.hpp>
#include <string.h>
#include <iostream>

#define RADIO_HEADER_LENGTH         2
#define RADIO_CRC_LENGTH            2
#define RADIO_OVERHEAD_LENGTH       (RADIO_HEADER_LENGTH + RADIO_CRC_LENGTH)
#define RADIO_MIN_LENGTH            (RADIO_OVERHEAD_LENGTH)
#define RADIO_MAX_LENGTH            64
#define RADIO_MAX_PAYLOAD_LENGTH    (RADIO_MAX_LENGTH - RADIO_OVERHEAD_LENGTH - 1)


Radio::Radio(Si4432 * transceiver, uint8_t remote_address)
{
    _transceiver  = transceiver;
    _address      = remote_address;
}

void Radio::send(uint8_t channel, ByteArray payload)
{
    ByteArray packet = ByteArray();
    Crc16     crc(0);

    if (payload.length() <= RADIO_MAX_PAYLOAD_LENGTH)
    {
        /* Add the radio header and CRC to build the packet */
        packet.append(_address | 0x80);
        packet.append(channel);
        packet.append(payload);
        crc.update(payload.length() + RADIO_OVERHEAD_LENGTH);
        crc.update(packet.data(), packet.length());
        packet.append(crc.get_msb());
        packet.append(crc.get_lsb());

        /* Ask the driver to send the packet */
        _transceiver->send_packet(packet);
    }
    else
    {
        std::cerr << "Invalid payload length : " << payload.length() << std::endl;
    }
}

tuple<uint8_t, ByteArray> Radio::receive(void)
{
    ByteArray packet;
    Crc16     crc(0);
    bool      direction;
    uint8_t   address;

    _transceiver->receive_packet(packet);

    if ((packet.length() > RADIO_MIN_LENGTH) && (packet.length() < RADIO_MAX_LENGTH))
    {
        direction = (bool)(packet(0) & 0x80);
        address   = packet(0) & 0x7f;

        crc.update(packet.length());
        crc.update(packet.data(), packet.length() - 2);

        if ((crc.get_msb() == packet(-2)) && (crc.get_lsb() == packet(-1)))
        {
            if ((direction == false) && (_address == address))
            {
                return tuple<int, ByteArray>{packet(1), packet.slice(2, packet.length() - RADIO_OVERHEAD_LENGTH)};
            }
        }
        else
        {
            std::cerr << "Bad CRC" << std::endl;
        }
    }

    return tuple<int, ByteArray>{0, ByteArray()};
}