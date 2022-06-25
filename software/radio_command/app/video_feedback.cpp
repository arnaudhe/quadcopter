#include "video_feedback.hpp"

#include <os/tick.hpp>

VideoFeedback::VideoFeedback(unsigned int width, unsigned int height, BatteryMonitor * battery, RadioCommand * radio) :
    Task("video", false),
    _framebuffer("/dev/fb0"),
    _video_capture("/dev/video0", width, height, VideoCapture::MJPEG),
    _camera_image(width, height),
    _infos_image(_framebuffer.width() - width, height),
    _battery(battery),
    _radio(radio)
{
    return;
}

void VideoFeedback::display_level(std::string name, int level, unsigned int row)
{
    _infos_image.display_text(row, 10, name, 255, 255, 255);
    _infos_image.display_rectangle(row + 30, 10, row + 58, _infos_image.width() - 10, 10, 10, 10);
    _infos_image.display_rectangle(row + 32, 12, row + 56, 12 + (level * (_infos_image.width() - 24)) / 100, 10, 128, 10);
    _infos_image.display_text(row + 32, _infos_image.width() / 2 - 20, std::to_string(level), 255, 255, 255);
}

void VideoFeedback::run()
{
    Tick telemety_tick = Tick::now();

    while (running())
    {
        if ((Tick::now() - telemety_tick) > 500)
        {
            _infos_image.fill(50, 50, 50);

            if (_radio->armed())
            {
                _infos_image.display_text(20, 10, "ARMED", 0, 255, 128);
            }
            else
            {
                _infos_image.display_text(20, 10, "DISARMD", 255, 0, 0);
            }

            if (_radio->lost())
            {
                _infos_image.display_text(70, 10, "LINK", 255, 255, 255);
                _infos_image.display_text(102, 10, "LOST !", 255, 0, 0);
            }
            else
            {
                display_level("LINK", _radio->link_quality(), 70);
            }

            display_level("QUAD", _radio->quadcopter_battery(), 170);
            display_level("REMOTE", _battery->level(), 270);
            display_level("CAMERA", _radio->camera_battery(), 370);

            if (_radio->camera_connected())
            {
                _infos_image.display_text(440, 10, "UP", 0, 255, 128);
            }
            else
            {
                _infos_image.display_text(440, 10, "DOWN", 255, 0, 0);
            }

            if (_radio->camera_recording())
            {
                _infos_image.display_text(440, 100, "REC", 255, 0, 0);
            }

            _framebuffer.update(&_infos_image, 0);
        }

        if (_video_capture.capture())
        {
            _camera_image.from_rgb(_video_capture.image());
            _framebuffer.update(&_camera_image, _infos_image.width());
        }
    }
}
