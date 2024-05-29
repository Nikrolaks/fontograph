#pragma once

#include "buffer.h"
#include "font.h"

#include <vector>

namespace fontograph::recycle {

struct ShapeResult {
    size_t glyphId;
    float offsetX, offsetY;
    float advanceX, advanceY;
};

std::vector<ShapeResult> shape(const Font& font, Buffer buffer);

} // namespace fontograph::recycle
