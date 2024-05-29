#include "font_storage.h"
#include "shaper.h"

#include <filesystem>
#include <memory>

namespace fontograph {

class FontManager {
public:
    FontManager(const std::filesystem::path& workingDirectory)
        : workingDirectory_(workingDirectory) { }

    std::shared_ptr<FontStorage> loadFont(const std::string& ttfName) {
        return std::make_shared<FontStorage>(workingDirectory_ / (ttfName + ".json"));
    }

    std::shared_ptr<Shaper> loadShaper(const std::string& ttfName) {
        return std::make_shared<Shaper>(workingDirectory_ / (ttfName + ".ttf"));
    }

private:
    std::filesystem::path workingDirectory_;
};

} // namespace fontograph
