#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace fontograph {

struct FontDescriptor {
    uint32_t margin, xHeight, ascender, capHeight, height;
    int32_t descender;
};

struct Glyph {
    uint32_t width, height;
    int32_t bearingX, bearingY;
    uint32_t advance;
    std::vector<uint8_t> texels;
};

class FontStorage {
public:
    FontStorage(const std::filesystem::path& glyphsPath);

    Glyph& glyph(uint32_t id) { return glyphMap_[id]; }

    uint32_t margin() const { return fontParams_.margin; }

    uint32_t ascender() const { return fontParams_.ascender; }

    uint32_t capHeight() const { return fontParams_.capHeight; }

    uint32_t xHeight() const { return fontParams_.xHeight; }

    int32_t descender() const { return fontParams_.descender; }

    uint32_t height() const { return fontParams_.height; }

private:
    std::map<uint32_t, Glyph> glyphMap_;
    FontDescriptor fontParams_;
};

} // namespace fontograph
