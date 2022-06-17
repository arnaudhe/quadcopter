#include "video_capture.hpp"

#include <stdio.h>
#include <errno.h>

#define VIDEO_CAPTURE_BUFFER_COUNT  4

VideoCapture::VideoCapture(const char * device, int width, int height, Format format) :
    _jpeg_decoder(width, height), _format(format), _image(width, height)
{
    _open_device(device);
    _init_device(device, width, height);
    _start_capturing();
}

int VideoCapture::_xioctl(int request, void *arg)
{
    int r;
    do
    {
        r = ioctl(_fd, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

void VideoCapture::_open_device(const char * dev_name)
{
    struct stat st;

    if (-1 == stat(dev_name, &st))
    {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode))
    {
        fprintf(stderr, "%s is no device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    _fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (-1 == _fd)
    {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void VideoCapture::_init_mmap(const char * dev_name)
{
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;

    memset(&req, 0, sizeof(req));
    req.count  = VIDEO_CAPTURE_BUFFER_COUNT;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    /* Initiate Memory Mapping */
    if (-1 == _xioctl(VIDIOC_REQBUFS, &req))
    {
        if (EINVAL == errno)
        {
            fprintf(stderr, "%s does not support memory mapping\n", dev_name);
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr, "VIDIOC_REQBUFS failed: %d, %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* video output requires at least two buffers, one displayed and one filled by the application */
    if (req.count < 2)
    {
        fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < req.count; ++i)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        /* Query the status of a buffer */
        if (-1 == _xioctl(VIDIOC_QUERYBUF, &buf))
        {
            fprintf(stderr, "VIDIOC_QUERYBUF failed: %d, %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        buffer buffer;
        buffer.length = buf.length;
        buffer.start  = mmap(NULL,                      /* start anywhere */
                             buf.length,                /* length */
                             PROT_READ | PROT_WRITE,    /* required */
                             MAP_SHARED,                /* recommended */
                             _fd,
                             buf.m.offset);

        if (MAP_FAILED == buffer.start)
        {
            fprintf(stderr, "mmap failed: %d, %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        _buffers.push_back(buffer);
    }
}

/* set video streaming format here(width, height, pixel format, cropping, scaling) */
void VideoCapture::_init_device(const char * dev_name, int width, int height)
{
    struct v4l2_capability cap;
    struct v4l2_format     fmt;

    if (-1 == _xioctl(VIDIOC_QUERYCAP, &cap))
    {
        if (EINVAL == errno)
        {
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr, "VIDIOC_QUERYCAP failed: %d, %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        fprintf(stderr, "%s is no video capture device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
        exit(EXIT_FAILURE);
    }

    memset(&fmt, 0, sizeof(fmt));

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = width;
    fmt.fmt.pix.height      = height;
    fmt.fmt.pix.pixelformat = (_format == Format::MJPEG) ? V4L2_PIX_FMT_MJPEG : V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_NONE;

    if (-1 == _xioctl(VIDIOC_S_FMT, &fmt))
    {
        fprintf(stderr, "VIDIOC_S_FMT failed: %d, %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    _init_mmap(dev_name);
}

void VideoCapture::_start_capturing()
{
    unsigned int       i;
    struct v4l2_buffer buf;
    enum v4l2_buf_type type;

    for (i = 0; i < _buffers.size(); ++i)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        /* enqueue an empty (capturing) or filled (output) buffer in the driver's incoming queue */
        if (-1 == _xioctl(VIDIOC_QBUF, &buf))
        {
            fprintf(stderr, "VIDIOC_QBUF failed: %d, %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Start streaming I/O */
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == _xioctl(VIDIOC_STREAMON, &type))
    {
        fprintf(stderr, "VIDIOC_STREAMON failed: %d, %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void VideoCapture::_close_device()
{
    close(_fd);
    _fd = -1;
}

void VideoCapture::_stop_capturing()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    _xioctl(VIDIOC_STREAMOFF, &type);
}

void VideoCapture::_uninit_device()
{
    unsigned int i;

    for (i = 0; i < _buffers.size(); ++i)
    {
        munmap(_buffers[i].start, _buffers[i].length);
    }

    _buffers.clear();
}

bool VideoCapture::capture(void)
{
    struct timeval tv;
    fd_set         fds;
    v4l2_buffer    buf;
    uint8_t      * image;
    size_t         image_size;

    FD_ZERO(&fds);
    FD_SET(_fd, &fds);

    /* Timeout. */
    tv.tv_sec  = 2;
    tv.tv_usec = 0;

    /* Wait on fd */
    select(_fd + 1, &fds, NULL, NULL, &tv);

    if (FD_ISSET(_fd, &fds))
    {
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        /* dequeue from buffer */
        if (-1 == _xioctl(VIDIOC_DQBUF, &buf))
        {
            switch (errno)
            {
                case EAGAIN:
                    return false;
                default:
                    fprintf(stderr, "VIDIOC_DQBUF failed: %d, %s\n", errno, strerror(errno));
                    return false;
            }
        }

        image      = (uint8_t *)_buffers[buf.index].start;
        image_size = _buffers[buf.index].length;

        if (_format == Format::MJPEG)
        {
            if (_jpeg_decoder.decode(image, image_size) == false)
            {
                fprintf(stderr, "jpeg decode error\n");
                return false;
            }
        }
        else    /* YUV */
        {
            fprintf(stderr, "not implemented\n");
            return false;
        }

        /* queue-in buffer */
        if (-1 == _xioctl(VIDIOC_QBUF, &buf))
        {
            fprintf(stderr, "VIDIOC_QBUF failed: %d, %s\n", errno, strerror(errno));
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        fprintf(stderr, "capture timeout\n");
        return false;
    }
}

VideoCapture::~VideoCapture()
{
    _stop_capturing();
    _uninit_device();
    _close_device();
}

const RGBPixMap * VideoCapture::image(void) const
{
    if (_format == Format::MJPEG)
    {
        return _jpeg_decoder.decoded();
    }
    else
    {
        return &_image;
    }
}
