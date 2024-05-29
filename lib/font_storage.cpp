#include "font_storage.h"

#include "utils.h"

#include <rapidjson/document.h>

#include <fstream>
#include <string>

namespace fontograph {

FontStorage::FontStorage(const std::filesystem::path& glyphsPath) {
    std::ifstream fontFile(glyphsPath);
    std::string fontJsonEntry(
        (std::istreambuf_iterator<char>(fontFile)), std::istreambuf_iterator<char>());
    fontFile.close();
    rapidjson::Document doc;

    doc.Parse(fontJsonEntry.c_str());

    const auto& fontDescription = doc["font"];
    const auto& glyphs = doc["glyphs"].GetArray();

    fontParams_.margin = fontDescription["margin"].GetInt();
    fontParams_.ascender = fontDescription["ascender"].GetInt();
    fontParams_.capHeight = fontDescription["capHeight"].GetInt();
    fontParams_.xHeight = fontDescription["xheight"].GetInt();
    fontParams_.descender = fontDescription["descender"].GetInt();
    fontParams_.height = fontDescription["height"].GetInt();

    for (const auto& glyph: glyphs) {
        glyphMap_.insert(
            {glyph["id"].GetInt(),
             Glyph{
                 .width = uint32_t(glyph["width"].GetInt()),
                 .height = uint32_t(glyph["height"].GetInt()),
                 .bearingX = int32_t(glyph["bearingX"].GetInt()),
                 .bearingY = int32_t(glyph["bearingY"].GetInt()),
                 .advance = uint32_t(glyph["advance"].GetInt()),
                 .texels = base64_decode(glyph["bitmap"].GetString())}});
    }
}

} // namespace fontograph
