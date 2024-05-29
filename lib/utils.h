#pragma once

#include <exception>
#include <filesystem>
#include <vector>

namespace fontograph {

#define REQUIRE(condition, errorMessage)  \
    if (!(condition)) {                   \
        std::runtime_error(errorMessage); \
    }

std::vector<uint8_t> base64_decode(std::string const& encoded_string);

void savePng(uint8_t* data, const std::filesystem::path& fpath, uint32_t width, uint32_t height);

} // namespace fontograph
