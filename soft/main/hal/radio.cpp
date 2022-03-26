#include <hal/radio.h>
#include <os/task.h>
#include <hal/log.h>
#include <utils/crc16.h>

RadioBroker::RadioBroker(float period, uint8_t address, Si4432 * transceiver):
    PeriodicTask("camera_controller", Task::Priority::MEDIUM, (int)(period * 1000), false)
{
    _transceiver = transceiver;
    _mutex       = new Mutex();
    _address     = address;

    for (int i = 0; i < RADIO_REGISTRATION_COUNT; i++)
    {
        _registrations[i].registered = false;
    } 
}

void RadioBroker::run()
{
    uint8_t length;
    uint8_t packet[64];
    Crc16 crc(0);

    _mutex->lock();
    _transceiver->receive_packet(packet, &length);
    _mutex->unlock();

    if ((length) && (length < 60))
    {
        LOG_VERBOSE("Received (%d) packet", length);

        bool direction = (bool)(packet[0] & 0x80);
        uint8_t address = packet[0] & 0x7f;
        uint8_t channel = packet[1];

        crc.update(&length, 1);
        crc.update(packet, length - 2);

        if ((crc.get_msb() == packet[length - 2]) && (crc.get_lsb() == packet[length - 1]))
        {
            if ((direction == true) && (_address == address))
            {
                LOG_VERBOSE("channel = %d", channel);
                _mutex->lock();
                for (int i = 0; i < RADIO_REGISTRATION_COUNT; i++)
                {
                    if (_registrations[i].registered && _registrations[i].channel == channel)
                    {
                        _registrations[i].received_frame_pending = true;
                        _registrations[i].received_frame_length = length - 4;
                        memcpy(_registrations[i].received_frame, &packet[2], length - 4);
                        break;
                    }
                }
                _mutex->unlock();
            }
        }
        else
        {
            LOG_WARNING("Bad CRC (%02x%02x vs %02x%02x)", crc.get_msb(), crc.get_lsb(), packet[length - 2], packet[length - 1]);
        }
    }
}

void RadioBroker::register_channel(uint8_t channel)
{
    _mutex->lock();

    for (int i = 0; i < RADIO_REGISTRATION_COUNT; i++)
    {
        if (_registrations[i].registered == false)
        {
            LOG_INFO("Register channel %d at %d", channel, i);
            _registrations[i].registered = true;
            _registrations[i].channel = channel;
            _registrations[i].received_frame_pending = false;
            break;
        }
    }

    _mutex->unlock();
}

bool RadioBroker::received_frame_pending(uint8_t channel)
{
    bool result = false;

    _mutex->lock();

    for (int i = 0; i < RADIO_REGISTRATION_COUNT; i++)
    {
        if (_registrations[i].registered && _registrations[i].channel == channel)
        {
            result = _registrations[i].received_frame_pending;
            break;
        }
    }

    _mutex->unlock();

    return result;
}

bool RadioBroker::receive(uint8_t channel, uint8_t * data, uint8_t * length)
{
    bool result = false;

    _mutex->lock();

    for (int i = 0; i < RADIO_REGISTRATION_COUNT; i++)
    {
        if (_registrations[i].registered && _registrations[i].channel == channel)
        {
            if (_registrations[i].received_frame_pending)
            {
                *length = _registrations[i].received_frame_length;
                memcpy(data, _registrations[i].received_frame, _registrations[i].received_frame_length);
                _registrations[i].received_frame_pending = false;
                _registrations[i].received_frame_length = 0;
                result = true;
            }
            else
            {
                *length = 0;
            }
            break;
        }
    }

    _mutex->unlock();

    return result;
}

void RadioBroker::send(uint8_t channel, uint8_t * data, uint8_t length)
{
    uint8_t packet[64];
    Crc16 crc(0);

    if (length < 60)
    {
        packet[0] = length + 4;
        packet[1] = _address;
        packet[2] = channel;
        memcpy(&packet[3], data, length);
        crc.update(packet, length + 3);
        packet[length + 3] = crc.get_msb();
        packet[length + 4] = crc.get_lsb();
        _mutex->lock();
        _transceiver->send_packet(packet, packet[0] + 1);
        _mutex->unlock();
    }
    else
    {
        LOG_ERROR("Invalid length %d", length);
    }
}
