#include "utils.h"

#include <harfbuzz/hb.h>
#include <png.h>

#include <cstdio>
#include <exception>
#include <fstream>
#include <string>
#include <vector>

namespace fontograph {

namespace {

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

static inline bool is_base64(uint8_t c) {
    return (isalnum(c) || (c == uint8_t('+')) || (c == uint8_t('/')));
}

} // namespace

std::vector<uint8_t> base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    uint8_t char_array_4[4], char_array_3[3];
    std::vector<uint8_t> ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret.push_back(char_array_3[j]);
    }

    return ret;
}

void savePng(uint8_t* data, const std::filesystem::path& fpath, uint32_t width, uint32_t height) {
    std::vector<uint8_t*> rows(height);
    for (std::size_t i = 0; i < height; ++i) {
        rows[i] = data + width * i * 4;
    }

    FILE* fp = fopen(fpath.c_str(), "wb");
    if (!fp) {
        throw std::runtime_error("Can't open file");
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        goto closeFile;
    }

    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        goto destroyWriteStruct;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        goto destroyWriteStruct;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(
        png_ptr,
        png_info,
        width,
        height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);

    png_set_rows(png_ptr, png_info, rows.data());
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, nullptr);
    png_write_end(png_ptr, png_info);

destroyWriteStruct:
    png_destroy_write_struct(&png_ptr, nullptr);

closeFile:
    fclose(fp);
}

} // namespace fontograph
