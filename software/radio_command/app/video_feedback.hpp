#pragma once

#include <os/task.hpp>

#include <hal/framebuffer.hpp>
#include <hal/video_capture.hpp>

#include <utils/pixmap.hpp>

class VideoFeedback : public Task
{

private:

    FrameBuffer  _framebuffer;
    VideoCapture _video_capture;
    RGB565PixMap _image;

    void run(void);

public:

    VideoFeedback(unsigned int width, unsigned int height);

};