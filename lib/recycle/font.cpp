#include "font.h"

#include <harfbuzz/hb.h>

namespace fontograph::recycle {

Font Font::fromFace(Face&& face) {
    auto rawFace = face.get();
    return Font(std::move(face), hb_font_create(rawFace));
}

void Font::Deleter::operator()(hb_font_t* font) const {
    //https://harfbuzz.github.io/object-model-lifecycle.html
    //"It is also permissible to reference() or to destroy() the NULL value"
    hb_font_destroy(font);
}

} // namespace fontograph::recycle
