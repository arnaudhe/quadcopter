#include <iostream>
#include <platform.hpp>
#include <periph/i2c.hpp>
#include <drv/ADS1115.hpp>
#include <app/video_feedback.hpp>
#include <app/radio_command.hpp>
#include <app/battery_monitor.hpp>

int main(int argc, char ** argv)
{
    I2C            i2c     = I2C(PLATFORM_BATTERY_I2C);
    ADS1115        ads     = ADS1115(&i2c);
    BatteryMonitor battery = BatteryMonitor(&ads, PLATFORM_BATTERY_CHANNEL);
    VideoFeedback  video   = VideoFeedback(640, 480);
    RadioCommand   rc      = RadioCommand();

    video.start();
    rc.start();
    battery.start();

    std::cout << "Ready !" << std::endl;

    rc.stop();
    video.stop();
    battery.stop();

    return 0;
}
