#pragma once

#include "blob.h"

struct hb_face_t;

namespace fontograph::recycle {

class Face {
public:
    Face() = default;

    static Face fromBlob(Blob&& blob);

    hb_face_t* get() const { return face_.get(); }

private:
    struct Deleter {
        void operator()(hb_face_t*) const;
    };

    explicit Face(Blob&& blob, hb_face_t* face)
        : blob_(std::move(blob))
        , face_(face) { }

    Blob blob_;
    std::unique_ptr<hb_face_t, Deleter> face_;
};

} // namespace fontograph::recycle
