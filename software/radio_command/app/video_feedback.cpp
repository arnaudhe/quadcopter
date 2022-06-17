#include "video_feedback.hpp"

VideoFeedback::VideoFeedback(unsigned int width, unsigned int height) :
    Task("video", false),
    _framebuffer("/dev/fb0"),
    _video_capture("/dev/video0", width, height, VideoCapture::MJPEG),
    _image(width, height)
{
    return;
}

void VideoFeedback::run()
{
    while (running())
    {
        if (_video_capture.capture())
        {
            _image.from_rgb(_video_capture.image());
            _framebuffer.update(&_image);
        }
    }
}
