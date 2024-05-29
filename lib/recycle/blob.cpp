#include "blob.h"

#include <harfbuzz/hb.h>

namespace fontograph::recycle {

void Blob::Deleter::operator()(hb_blob_t* b) const {
    //https://harfbuzz.github.io/object-model-lifecycle.html
    //"It is also permissible to reference() or to destroy() the NULL value"
    hb_blob_destroy(b);
}

Blob Blob::fromData(std::vector<char> data) {
    auto* rawPtr = data.data();
    auto size = data.size();
    return Blob(
        std::move(data), hb_blob_create(rawPtr, size, HB_MEMORY_MODE_WRITABLE, nullptr, nullptr));
}

} // namespace fontograph::recycle
