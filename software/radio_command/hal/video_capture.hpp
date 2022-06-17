#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>           /* low-level i/o control (open)*/
#include <errno.h>
#include <string.h>          /* strerror show errno meaning */
#include <sys/stat.h>        /* getting information about files attributes */
#include <linux/videodev2.h> /* v4l2 structure */
#include <sys/mman.h>        /* memory mapping */
#include <unistd.h>          /* read write close */
#include <sys/time.h>        /* for select time */
#include <limits.h>          /* for UCHAR_MAX */
#include <jpeglib.h>
#include <stdint.h>
#include <utils/pixmap.hpp>
#include <utils/jpeg_decoder.hpp>
#include <vector>

class VideoCapture
{

public:

    enum Format
    {
        MJPEG,  //< JPEG stream
        YUYV,   //< YUV 4:2:2
    };

private:

    struct buffer {
        void * start;
        size_t length;
    };

    /* video device file descriptor*/
    int _fd;

    /* Queried image buffers! */
    std::vector<buffer> _buffers;

    /* JPEG decoder for MJPEG video devices */
    JpegDecoder _jpeg_decoder;

    /* Video capture device output format */
    Format _format;

    /* Captured image pixmap */
    RGBPixMap _image;

    void _errno_exit(const char *s);
    int  _xioctl(int request, void *arg);
    void _open_device(const char * device);
    void _init_mmap(const char * dev_name);
    void _init_device(const char * dev_name, int width, int height);
    void _start_capturing();
    void _close_device();
    void _stop_capturing();
    void _uninit_device();

public:

    VideoCapture(const char * device, int width, int height, Format format);
    ~VideoCapture();

    bool capture(void);

    const RGBPixMap * image(void) const;

};
