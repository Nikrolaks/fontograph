#include "shaper.h"

#include "utils.h"

#include "recycle/blob.h"
#include "recycle/buffer.h"
#include "recycle/face.h"

#include <fstream>

namespace fontograph {

Shaper::Shaper(const std::filesystem::path& ttfPath) {
    std::ifstream fontFile(ttfPath);
    std::vector<char> fontEntry(
        (std::istreambuf_iterator<char>(fontFile)), std::istreambuf_iterator<char>());
    auto blob = recycle::Blob::fromData(fontEntry);
    auto face = recycle::Face::fromBlob(std::move(blob));
    font_ = recycle::Font::fromFace(std::move(face));
}

std::vector<recycle::ShapeResult> Shaper::shape(const std::u16string& message) const {
    return recycle::shape(font_, recycle::Buffer::fromUTF16(message));
}

} // namespace fontograph
