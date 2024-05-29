#include "face.h"

#include <harfbuzz/hb.h>

namespace fontograph::recycle {

Face Face::fromBlob(Blob&& blob) {
    auto rawBlob = blob.get();
    return Face(std::move(blob), hb_face_create(rawBlob, 0));
}

void Face::Deleter::operator()(hb_face_t* f) const {
    //https://harfbuzz.github.io/object-model-lifecycle.html
    //"It is also permissible to reference() or to destroy() the NULL value"
    hb_face_destroy(f);
}

} // namespace fontograph::recycle
