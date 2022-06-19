#include <app/workers/broker.h>
#include <os/task.h>
#include <hal/log.h>
#include <platform.h>

Broker::Broker(float period, Radio * radio):
    PeriodicTask("broker", Task::Priority::MEDIUM, 16 * 1024, (int)(period * 1000), false)
{
    _radio      = radio;
    _mutex      = new Mutex();
    _udp_sender = new Udp(PLATFORM_UDP_PORT_BASE + 42);

    for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
    {
        _registrations[i].registered = false;
    }
}

void Broker::run()
{
    Channel   channel;
    ByteArray frame;
    int       rssi;

    tuple<uint8_t, ByteArray, int> radio_recv = _radio->receive();
    channel = (Channel)(get<0>(radio_recv));
    frame = get<1>(radio_recv);
    rssi = get<2>(radio_recv);

    _mutex->lock();

    if (frame.length())
    {
        for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
        {
            if ((_registrations[i].registered == true) && (_registrations[i].channel == channel) &&
               ((_registrations[i].medium == Medium::RADIO) || (_registrations[i].medium == Medium::UDP_AND_RADIO)))
            {
                _registrations[i].received_frame_pending = true;
                _registrations[i].received_frame_rssi    = rssi;
                _registrations[i].received_frame         = frame;
                _registrations[i].received_frame_medium  = Medium::RADIO;
                _registrations[i].received_frame_address = "(radio)";
                break;
            }
        }
    }

    for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
    {
        if ((_registrations[i].registered == true) && ((_registrations[i].medium == Medium::UDP) || (_registrations[i].medium == Medium::UDP_AND_RADIO)))
        {
            tuple<ByteArray, string, int> udp_recv = _registrations[i].udp_listener->receive_from(256);
            frame = get<0>(udp_recv);
            if (frame.length())
            {
                _registrations[i].received_frame_pending = true;
                _registrations[i].received_frame_rssi    = 0;
                _registrations[i].received_frame         = frame;
                _registrations[i].received_frame_address = get<1>(udp_recv);
                _registrations[i].received_frame_medium  = Medium::UDP;
            }
        }
    }

    _mutex->unlock();
}

void Broker::register_channel(Channel channel, Medium medium)
{
    _mutex->lock();

    for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
    {
        if (_registrations[i].registered == false)
        {
            LOG_INFO("Register channel %d at %d", channel, i);
            _registrations[i].registered = true;
            _registrations[i].channel = channel;
            _registrations[i].medium = medium;
            _registrations[i].received_frame_pending = false;
            if ((medium == Medium::UDP) || (medium == Medium::UDP_AND_RADIO))
            {
                _registrations[i].udp_listener = new Udp(PLATFORM_UDP_PORT_BASE + (int)channel);
            }
            else
            {
                _registrations[i].udp_listener = NULL;
            }
            break;
        }
    }

    _mutex->unlock();
}

bool Broker::received_frame_pending(Channel channel)
{
    bool result = false;

    _mutex->lock();

    for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
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

ByteArray Broker::receive(Channel channel)
{
    ByteArray ret;

    _mutex->lock();

    for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
    {
        if (_registrations[i].registered && _registrations[i].channel == channel)
        {
            if (_registrations[i].received_frame_pending)
            {
                _registrations[i].received_frame_pending = false;
                ret = _registrations[i].received_frame;
            }
            break;
        }
    }

    _mutex->unlock();

    return ret;
}

tuple<ByteArray, int> Broker::receive_rssi(Channel channel)
{
    tuple<ByteArray, int> ret = {ByteArray(), 0};

    _mutex->lock();

    for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
    {
        if (_registrations[i].registered && _registrations[i].channel == channel)
        {
            if (_registrations[i].received_frame_pending)
            {
                _registrations[i].received_frame_pending = false;
                ret = tuple<ByteArray, int>{_registrations[i].received_frame, _registrations[i].received_frame_rssi};
            }
            break;
        }
    }

    _mutex->unlock();

    return ret;
}

tuple<ByteArray, Broker::Medium, string> Broker::receive_from(Channel channel)
{
    tuple<ByteArray, Broker::Medium, string> ret = {ByteArray(), Medium::NONE, "(none)"};

    _mutex->lock();

    for (int i = 0; i < BROKER_REGISTRATION_COUNT; i++)
    {
        if (_registrations[i].registered && _registrations[i].channel == channel)
        {
            if (_registrations[i].received_frame_pending)
            {
                _registrations[i].received_frame_pending = false;
                ret = tuple<ByteArray, Broker::Medium, string>{_registrations[i].received_frame, _registrations[i].received_frame_medium, _registrations[i].received_frame_address};
            }
            break;
        }
    }

    _mutex->unlock();

    return ret;
}

void Broker::send(Channel channel, Medium medium, ByteArray data)
{
    if ((medium == Medium::RADIO) || (medium == Medium::UDP_AND_RADIO))
    {
        _radio->send((uint8_t)channel, data);
    }

    if ((medium == Medium::UDP) || (medium == Medium::UDP_AND_RADIO))
    {
        _udp_sender->send_broadcast(data, PLATFORM_UDP_PORT_BASE + (int)channel);
    }
}

void Broker::send_to(Channel channel, ByteArray data, string address)
{
    if (address == "(radio)")
    {
        _radio->send((uint8_t)channel, data);
    }
    else
    {
        _udp_sender->send_to(data, address, PLATFORM_UDP_PORT_BASE + (int)channel);
    }
}
