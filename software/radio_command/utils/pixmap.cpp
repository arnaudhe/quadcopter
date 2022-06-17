#include "pixmap.hpp"

PixMap::PixMap(unsigned int width, unsigned int height, ColorSpace colorspace)
{
    _width      = width;
    _height     = height;
    _colorspace = colorspace;

    if (_colorspace == ColorSpace::RGB565)
    {
        _bytes_per_pixel = 2;
    }
    else
    {
        _bytes_per_pixel = 3;
    }

    _data.resize(this->size());
}

int PixMap::_compute_offset(int row, int column) const
{
    return ((row * _width + column) * _bytes_per_pixel);
}

size_t PixMap::size(void) const
{
    return _width * _height * _bytes_per_pixel;
}

unsigned int PixMap::width(void) const
{
    return _width;
}

unsigned int PixMap::height(void) const
{
    return _height;
}

PixMap::ColorSpace PixMap::colorspace(void) const
{
    return _colorspace;
}

unsigned int PixMap::bytes_per_pixel(void) const
{
    return _bytes_per_pixel;
}

const uint8_t * PixMap::row(unsigned int row) const
{
    return &_data.data()[this->_compute_offset(row, 0)];
}

const uint8_t * PixMap::operator()(unsigned int row)
{
    return this->row(row);
}

const uint8_t * PixMap::at(unsigned int row, unsigned int column) const
{
    return &_data.data()[this->_compute_offset(row, column)];
}

const uint8_t * PixMap::operator()(unsigned int row, unsigned int column)
{
    return this->at(row, column);
}

void RGBPixMap::set(unsigned int row, unsigned int column, uint8_t red, uint8_t green, uint8_t blue)
{
    if ((row < _height) && (column < _width))
    {
        uint8_t * pixel = (uint8_t *)this->at(row, column);

        pixel[0] = red;
        pixel[1] = green;
        pixel[2] = blue;
    }
}

void RGB565PixMap::set(unsigned int row, unsigned int column, uint8_t red, uint8_t green, uint8_t blue)
{
    if ((row < _height) && (column < _width))
    {
        uint16_t * pixel = (uint16_t *)this->at(row, column);

        uint16_t b = (blue >> 3) & 0x1f;
        uint16_t g = ((green >> 2) & 0x3f) << 5;
        uint16_t r = ((red >> 3) & 0x1f) << 11;

        *pixel = (uint16_t) (r | g | b);
    }
}

void RGB565PixMap::from_rgb(const RGBPixMap * rgb)
{
    if (rgb == nullptr)
    {
        return;
    }

    if ((rgb->width() != _width) || (rgb->height() != _height))
    {
        return;
    }

    for (unsigned int row = 0; row < _height; row++)
    {
        for (unsigned int column = 0; column < _width; column++)
        {
            const uint8_t * pixel = rgb->at(row, column);
            this->set(row, column, pixel[0], pixel[1], pixel[2]);
        }
    }
}
