#include "framebuffer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

FrameBuffer::FrameBuffer(const char * device)
{
    fb_var_screeninfo vinfo;
    fb_fix_screeninfo finfo;

    _fd = open(device, O_RDWR);

    if (_fd == -1)
    {
        perror("Error opening framebuffer device");
        exit(EXIT_FAILURE);
    }

    /* Get fixed screen information */
    if (ioctl(_fd, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
        perror("Error reading fixed information");
        exit(EXIT_FAILURE);
    }

    /* Get variable screen information */
    if (ioctl(_fd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror("Error reading variable information");
        exit(EXIT_FAILURE);
    }

    _xoffset         = vinfo.xoffset;
    _yoffset         = vinfo.yoffset;
    _width           = vinfo.xres;
    _height          = vinfo.yres;
    _bytes_per_pixel = vinfo.bits_per_pixel / 8;
    _line_length     = finfo.line_length;

    _fbp = (uint8_t *)mmap(0, this->size(), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (_fbp == (void *)-1)
    {
        perror("Error mapping framebuffer device to memory");
        exit(EXIT_FAILURE);
    }
}

int FrameBuffer::size(void)
{
    return (_width * _height * _bytes_per_pixel);
}

void FrameBuffer::update(RGB565PixMap * image)
{
    unsigned int row;
    unsigned int location = 0;

    if (image == nullptr)
    {
        return;
    }

    int x_padding = (this->width() - image->width()) / 2;
    int y_padding = (this->height() - image->height()) / 2;

    for (row = y_padding; row < image->height(); row++)
    {
        location = ((x_padding + _xoffset) * _bytes_per_pixel) + ((row + _yoffset) * _line_length);
        memcpy(&_fbp[location], image->row(row), image->width() * _bytes_per_pixel);
    }
}

int FrameBuffer::width(void)
{
    return _width;
}

int FrameBuffer::height(void)
{
    return _height;
}

FrameBuffer::~FrameBuffer(void)
{
    munmap(_fbp, this->size());
    close(_fd);
}