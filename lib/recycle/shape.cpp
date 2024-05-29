#include "shape.h"

#include <harfbuzz/hb.h>

#include <array>

namespace fontograph::recycle {

namespace {
float from_fixed_26_6(hb_position_t value) {
    // Dividing by 64 since hb_position_t is in 26.6 fixed-point format
    return value / 64.f;
}

auto createShapeFeatures() {
    std::array<hb_feature_t, 1> features;
    hb_feature_from_string("liga", -1, &features[0]);
    return features;
}
} // namespace

std::vector<ShapeResult> shape(const Font& font, Buffer buffer) {
    static const auto features = createShapeFeatures();

    hb_shape(font.get(), buffer.get(), features.data(), features.size());

    unsigned int glyphCount;
    hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer.get(), &glyphCount);
    hb_glyph_position_t* glyphPosition = hb_buffer_get_glyph_positions(buffer.get(), nullptr);

    std::vector<ShapeResult> result(glyphCount);
    for (unsigned int i = 0; i < glyphCount; ++i) {
        result[i].glyphId = glyphInfo[i].codepoint;
        result[i].offsetX = from_fixed_26_6(glyphPosition[i].x_offset);
        result[i].offsetY = from_fixed_26_6(glyphPosition[i].y_offset);
        result[i].advanceX = from_fixed_26_6(glyphPosition[i].x_advance);
        result[i].advanceY = from_fixed_26_6(glyphPosition[i].y_advance);
    }
    return result;
}

} // namespace fontograph::recycle
