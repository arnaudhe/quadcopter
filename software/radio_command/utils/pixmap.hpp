#pragma once

#include <stdint.h>
#include <vector>

class PixMap
{

public:

    enum ColorSpace {
        RGB,
        YUV,
        RGB565
    };

protected:

    unsigned int          _width;
    unsigned int          _height;
    ColorSpace            _colorspace;
    int                   _bytes_per_pixel;
    std::vector<uint8_t>  _data;

    int _compute_offset(int row, int column) const;

public:

    PixMap(unsigned int width, unsigned int height, ColorSpace colorspace);

    /* Total pixmap size in memory */
    size_t size(void) const;

    /* Fixmap width (number of columns) */
    unsigned int width(void) const;

    /* Fixmap height (number of rows) */
    unsigned int height(void) const;

    /* Fixmap colorspace */
    ColorSpace colorspace(void) const;

    /* Depth of pixel, in bytes */
    unsigned int bytes_per_pixel(void) const;

    /* Get pointer to start of row data */
    const uint8_t * row(unsigned int row) const;
    const uint8_t * operator()(unsigned int row);

    /* Get pointer to pixel data */
    const uint8_t * at(unsigned int row, unsigned int column) const;
    const uint8_t * operator()(unsigned int row, unsigned int column);

};

class RGBPixMap : public PixMap
{

public:

    RGBPixMap(unsigned int width, unsigned int height) : PixMap(width, height, ColorSpace::RGB) {}

    void set(unsigned int row, unsigned int column, uint8_t red, uint8_t green, uint8_t blue);
};

class RGB565PixMap : public PixMap
{

public:

    RGB565PixMap(unsigned int width, unsigned int height) : PixMap(width, height, ColorSpace::RGB565) {}

    void set(unsigned int row, unsigned int column, uint8_t red, uint8_t green, uint8_t blue);

    void from_rgb(const RGBPixMap * rgb);

};
