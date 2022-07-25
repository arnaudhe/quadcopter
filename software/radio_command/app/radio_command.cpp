#include "radio_command.hpp"

#include <platform.hpp>

#include <os/tick.hpp>

#include <iostream>

#include <sys/time.h>

#define HEARTBEAT_CHANNEL       1
#define RADIO_COMMAND_CHANNEL   2
#define RADIO_COMMAND_ADDRESS   31

#define RADIO_COMMAND_PERIOD                50
#define RADIO_COMMAND_RANGE_TEST_PERIOD     1500
#define RADIO_COMMAND_SEND_PERIOD           100
#define RADIO_COMMAND_WAIT_AFTER_RX         40      ///< 2 driver periods
#define RADIO_COMMAND_LOST_PERIOD           5000


RadioCommand::RadioCommand(void):
    Task("rc", false),
    _button_top_left(PLATFORM_BUTTON_TOP_LEFT),
    _button_bottom_left(PLATFORM_BUTTON_BOTTOM_LEFT),
    _button_bottom_right(PLATFORM_BUTTON_BOTTOM_RIGHT),
    _button_top_right(PLATFORM_BUTTON_TOP_RIGHT),
    _stick_yaw(PLATFORM_YAW_STICK_ADC_CHANNEL, 9638, 4200, 1.0, 1.0, 50.0, 140.0),
    _stick_throttle(PLATFORM_THROTTLE_STICK_ADC_CHANNEL, 8902, 4096, -1.0, 1.2, 50.0, 140.0),
    _stick_roll(PLATFORM_ROLL_STICK_ADC_CHANNEL, 7585, 3924, 1.0, 0.2, 50.0, 140.0),
    _stick_pitch(PLATFORM_PITCH_STICK_ADC_CHANNEL, 9711, 3535, -1.0, 0.2, 50.0, 140.0),
    _si4432(),
    _radio(&_si4432, RADIO_COMMAND_ADDRESS),
    _lost(false),
    _armed(false),
    _link_quality(0),
    _quadcopter_battery(0),
    _camera_battery(0),
    _camera_connected(false),
    _camera_recording(false)
{
    return;
}

void RadioCommand::run(void)
{
    CommandsPayload   payload;
    StatusPayload   * status;
    bool              armed = false;
    bool              arming = false;
    bool              range_test = false;
    bool              record = false;
    bool              record_press = false;
    Tick              rx_tick = Tick::now();
    Tick              tx_tick = Tick::now();

    if (_button_bottom_left.is_pressed() && _button_bottom_right.is_pressed())
    {
        std::cout << "Range test" << std::endl;
        range_test = true;
    }

    while (running())
    {
        if (_button_top_left.is_pressed() && _button_top_right.is_pressed())
        {
            if (arming == false)
            {
                arming = true;
                armed = !armed;
                if (armed)
                {
                    std::cout << "ARMED !" << std::endl;
                }
                else
                {
                    std::cout << "DISARMED !" << std::endl;
                }
            }
        }
        else
        {
            arming = false;
        }

        if (_button_bottom_right.is_pressed())
        {
            if (record_press == false)
            {
                record_press = true;
                record = !record;
            }
        }
        else
        {
            record_press = false;
        }

        tuple<uint8_t, ByteArray> recv = _radio.receive();
        uint8_t   recv_channel = get<0>(recv);
        ByteArray recv_packet  = get<1>(recv);

        if (recv_packet.length())   /* A packet have been received */
        {
            if ((recv_channel == RADIO_COMMAND_CHANNEL) && (recv_packet.length() == sizeof(StatusPayload)))
            {
                /* This packet is a valid rc status, unpack its data */
                status = (StatusPayload *)recv_packet.data();

                _armed              = status->armed;
                _quadcopter_battery = status->battery;
                _link_quality       = status->link_quality;
                _camera_recording   = status->recording;
                _camera_connected   = status->camera_connected;
                _camera_battery     = status->camera_battery;
            }

            rx_tick = Tick::now();
            _lost = false;
        }
        else if ((Tick::now() - rx_tick).ticks() > RADIO_COMMAND_LOST_PERIOD)
        {
            _lost = true;
        }

        if (range_test)
        {
            if (((Tick::now() - rx_tick).ticks() >= RADIO_COMMAND_WAIT_AFTER_RX) && ((Tick::now() - tx_tick).ticks() >= RADIO_COMMAND_RANGE_TEST_PERIOD))
            {
                _radio.send(HEARTBEAT_CHANNEL, ByteArray("range-test"));
            }
        }
        else
        {
            if (((Tick::now() - rx_tick).ticks() >= RADIO_COMMAND_WAIT_AFTER_RX) && ((Tick::now() - tx_tick).ticks() >= RADIO_COMMAND_SEND_PERIOD))
            {
                payload.armed       = armed;
                payload.roll        = _stick_roll.get_command();
                payload.pitch       = _stick_pitch.get_command();
                payload.yaw         = _stick_yaw.get_command();
                payload.throttle    = _stick_throttle.get_command();
                payload.camera_tilt = 0;
                payload.record      = record;

                (void)payload;

                _radio.send(RADIO_COMMAND_CHANNEL, ByteArray((const uint8_t *)&payload, (int)sizeof(payload)));

                tx_tick = Tick::now();
            }
        }

        Task::delay_ms(RADIO_COMMAND_PERIOD);
    }
}

void RadioCommand::start(void)
{
    _si4432.start();
    Task::start();
}

bool RadioCommand::lost(void)
{
    return _lost;
}

bool RadioCommand::armed(void)
{
    return _armed;
}

int RadioCommand::link_quality(void)
{
    return _link_quality;
}

int RadioCommand::quadcopter_battery(void)
{
    return _quadcopter_battery;
}

int RadioCommand::camera_battery(void)
{
    return _camera_battery;
}

bool RadioCommand::camera_connected(void)
{
    return _camera_connected;
}

bool RadioCommand::camera_recording(void)
{
    return _camera_recording;
}
