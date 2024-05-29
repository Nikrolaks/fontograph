#pragma once

#include "face.h"

struct hb_font_t;

namespace fontograph::recycle {

class Font {
public:
    Font() = default;

    static Font fromFace(Face&& face);

    hb_font_t* get() const { return font_.get(); }

private:
    struct Deleter {
        void operator()(hb_font_t*) const;
    };

    explicit Font(Face&& face, hb_font_t* font)
        : face_(std::move(face))
        , font_(font) { }

    Face face_;
    std::unique_ptr<hb_font_t, Deleter> font_;
};

} // namespace fontograph::recycle
