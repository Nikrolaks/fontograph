#pragma once

#include "recycle/font.h"
#include "recycle/shape.h"

#include <filesystem>
#include <string>

namespace fontograph {

class Shaper {
public:
    Shaper(const std::filesystem::path& ttfPath);
    std::vector<recycle::ShapeResult> shape(const std::u16string& message) const;

private:
    recycle::Font font_;
};

} // namespace fontograph
