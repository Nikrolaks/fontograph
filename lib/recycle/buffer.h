#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

struct hb_buffer_t;

namespace fontograph::recycle {

class Buffer {
public:
    Buffer() = default;

    static Buffer fromUTF16(std::u16string_view str);

    hb_buffer_t* get() const { return buffer_.get(); }

private:
    struct Deleter {
        void operator()(hb_buffer_t*) const;
    };

    explicit Buffer(hb_buffer_t* b)
        : buffer_(b) { }

    std::unique_ptr<hb_buffer_t, Deleter> buffer_;
};

} // namespace fontograph::recycle
