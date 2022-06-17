#include "jpeg_decoder.hpp"

#include <stdio.h>
#include <alloca.h>
#include <string.h>

JpegDecoder::JpegDecoder(unsigned int width, unsigned int height) :
    _decoded(width, height)
{
    _cinfo.err = jpeg_std_error(&_jerr);
    jpeg_create_decompress(&_cinfo);
}

bool JpegDecoder::decode(uint8_t * jpeg, size_t jpeg_size)
{
    uint8_t * lines[1];

    jpeg_mem_src(&_cinfo, jpeg, jpeg_size);

    if (jpeg_read_header(&_cinfo, TRUE) != JPEG_HEADER_OK)
    {
        fprintf(stderr, "Invalid jpeg frame, drop\n");
        return false;
    }

    jpeg_start_decompress(&_cinfo);

    if (_cinfo.output_height != _decoded.height())
    {
        fprintf(stderr, "jpeg height %d differs rgb height %d \n", _cinfo.output_height, _decoded.height());
        jpeg_finish_decompress(&_cinfo);
        return false;
    }

    if (_cinfo.output_width != _decoded.width())
    {
        printf("jpeg width %d differs rgb width %d \n", _cinfo.output_width, _decoded.width());
        jpeg_finish_decompress(&_cinfo);
        return false;
    }

    while (_cinfo.output_scanline < _decoded.height())
    {
        lines[0] = (uint8_t *)_decoded.row(_cinfo.output_scanline);
        jpeg_read_scanlines(&_cinfo, lines, 1);
        memset(lines[0], 0x7F, _decoded.bytes_per_pixel() * _decoded.bytes_per_pixel());
    }

    jpeg_finish_decompress(&_cinfo);

    return true;
}

const RGBPixMap * JpegDecoder::decoded(void) const
{
    return &_decoded;
}
