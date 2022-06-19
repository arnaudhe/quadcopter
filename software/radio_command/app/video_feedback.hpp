#pragma once

#include <string>

#include <os/task.hpp>

#include <hal/framebuffer.hpp>
#include <hal/video_capture.hpp>

#include <utils/pixmap.hpp>

#include <app/battery_monitor.hpp>
#include <app/radio_command.hpp>

class VideoFeedback : public Task
{

private:

    FrameBuffer      _framebuffer;
    VideoCapture     _video_capture;
    RGB565PixMap     _camera_image;
    RGB565PixMap     _infos_image;
    BatteryMonitor * _battery;
    RadioCommand   * _radio;

    void run(void);

    void display_level(std::string name, int level, unsigned int row);

public:

    VideoFeedback(unsigned int width, unsigned int height, BatteryMonitor * battery, RadioCommand * radio);

};