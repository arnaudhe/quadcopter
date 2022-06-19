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

void PixMap::fill(uint8_t red, uint8_t green, uint8_t blue)
{
    for (unsigned int row = 0; row < height(); row++)
    {
        for (unsigned int column = 0; column < width(); column++)
        {
            set(row, column, red, green, blue);
        }
    }
}

void PixMap::display_text(unsigned int row, unsigned int column, std::string text, uint8_t red, uint8_t green, uint8_t blue)
{
    for (unsigned int i = 0; i < text.length(); i++)
    {
        int c = int(text.at(i));
        if ((c >= 32) && (c <= 126))
        {
            for (unsigned int y = 0; y < 13; y++)
            {
                for (unsigned int x = 0; x < 8; x++)
                {
                    if ((_letters[c - 32][y] >> x) & 0x1)
                    {
                        set(row + (2 * (13 - y)),     column + (i * 21) + (2 * (7 - x)),     red, green, blue);
                        set(row + (2 * (13 - y)) + 1, column + (i * 21) + (2 * (7 - x)),     red, green, blue);
                        set(row + (2 * (13 - y)),     column + (i * 21) + (2 * (7 - x)) + 1, red, green, blue);
                        set(row + (2 * (13 - y)) + 1, column + (i * 21) + (2 * (7 - x)) + 1, red, green, blue);
                    }
                }
            }
        }
    }
}

void PixMap::display_rectangle(unsigned int from_row, unsigned int from_column, unsigned int to_row, unsigned int to_column,
                               uint8_t red, uint8_t green, uint8_t blue)
{
    for (unsigned int row = from_row; row < to_row; row++)
    {
        for (unsigned int column = from_column; column < to_column; column++)
        {
            set(row, column, red, green, blue);
        }
    }
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
