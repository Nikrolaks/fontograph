#pragma once

#include <cstddef>
#include <memory>
#include <vector>

struct hb_blob_t;

namespace fontograph::recycle {

class Blob {
public:
    Blob() = default;

    static Blob fromData(std::vector<char> data);

    hb_blob_t* get() const { return blob_.get(); }

private:
    struct Deleter {
        void operator()(hb_blob_t*) const;
    };

    explicit Blob(std::vector<char> data, hb_blob_t* b)
        : data_(std::move(data))
        , blob_(b) { }

    std::vector<char> data_;
    std::unique_ptr<hb_blob_t, Deleter> blob_;
};

} // namespace fontograph::recycle
