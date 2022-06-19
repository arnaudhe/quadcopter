#pragma once

#include <stdint.h>
#include <linux/fb.h>
#include <utils/pixmap.hpp>
class FrameBuffer
{

private:

    int       _fd;
    uint8_t * _fbp;
    int       _xoffset;
    int       _yoffset;
    int       _width;
    int       _height;
    int       _bytes_per_pixel;
    int       _line_length;

public:

    FrameBuffer(const char * device);
    ~FrameBuffer(void);

    void update(RGB565PixMap * image, int x_position = -1);

    int width();

    int height();

    int size();

};