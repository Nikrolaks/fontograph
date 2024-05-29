#include "buffer.h"

#include <harfbuzz/hb.h>

namespace fontograph::recycle {

void Buffer::Deleter::operator()(hb_buffer_t* b) const {
    //https://harfbuzz.github.io/object-model-lifecycle.html
    //"It is also permissible to reference() or to destroy() the NULL value"
    hb_buffer_destroy(b);
}

Buffer Buffer::fromUTF16(std::u16string_view str) {
    Buffer result{hb_buffer_create()};

    // the text is already reordered, therefore harfbuzz should shape the text as if it is always LTR
    hb_buffer_set_direction(result.get(), HB_DIRECTION_LTR);

    hb_buffer_add_utf16(
        result.get(), reinterpret_cast<const uint16_t*>(str.data()), str.size(), 0, -1);

    hb_buffer_guess_segment_properties(result.get());
    return result;
}

} // namespace fontograph::recycle
