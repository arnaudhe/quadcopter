#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include <utils/pixmap.hpp>

class JpegDecoder
{

private:

    struct jpeg_decompress_struct _cinfo;
    struct jpeg_error_mgr         _jerr;
    RGBPixMap                     _decoded;

public:

    JpegDecoder(unsigned int width, unsigned int height);

    bool decode(uint8_t * jpeg_data, size_t jpeg_data_len);

    const RGBPixMap * decoded(void) const;
};