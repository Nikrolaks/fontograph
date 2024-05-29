#include "drawer.h"

#include <boost/program_options.hpp>

#include <codecvt>
#include <exception>
#include <iostream>
#include <locale>
#include <memory>

namespace fontograph {

void errorCallback(int error, const char* description) {
    (void)(error);
    std::cerr << std::string(description) << "\n";
}

RenderingParams fillRenderingParams(const boost::program_options::variables_map& optsResult) {
    fontograph::RenderingParams params;
    params.fontSize = optsResult["font-size"].as<float>();
    params.relativeOriginX = optsResult["relative-origin-x"].as<float>();
    params.relativeOriginY = optsResult["relative-origin-y"].as<float>();
    params.debugVerbose = optsResult["debug-verbose"].as<int>();
    params.showGlyphsBitmap = optsResult.contains("show-bitmap");

    return params;
}

} // namespace fontograph

int main(int argc, char* argv[]) try {
    namespace bop = boost::program_options;
    bop::options_description opts;

    opts.add_options() // ???
        ("help,h", "manual") // help message
        ("storage",
         bop::value<std::string>()->default_value("fonts")) // placement with font information
        ("font",
         bop::value<std::string>()->default_value("YSText-Regular"),
         "font name") // font used for drawing
        ("output",
         bop::value<std::string>()->default_value("result.png")) // output file path
        ("window-width",
         bop::value<uint32_t>()->default_value(1280)) // not using with flexible-sizes
        ("window-height",
         bop::value<uint32_t>()->default_value(1080)) // not using with flexible-sizes
        ("text",
         bop::value<std::string>()->default_value("Воднобуерачное")) // text to render
        ("font-size", bop::value<float>()->default_value(50.f)) // capHeight in pixels
        ("debug-verbose",
         boost::program_options::value<int>()->default_value(0)) // draw debug information
        ("show-bitmap",
         "draw bitmap texture instead of converting it to word") // another debug option
        ("no-flexible-sizes", "use fixed sizes instead") //
        ("relative-origin-x",
         bop::value<float>()->default_value(-0.95f)) // relative x coord of label left-bottom edge
        ("relative-origin-y",
         bop::value<float>()->default_value(-0.5f)) // relative y coord of label left-bottom edge
        ;

    // flexible sizes - auto sizes, auto centering
    // (rox, roy) in this case are parameters to calculate paddings
    // if flexible sizes enabled, (w-w, w-h) will be ignored

    bop::variables_map optsResult;

    bop::store(bop::command_line_parser(argc, argv).options(opts).run(), optsResult);
    bop::notify(optsResult);
    if (optsResult.contains("help")) {
        std::cout << argv[0] << " [options] <" << opts << std::endl;
        return 0;
    }

    if (!glfwInit()) {
        throw std::runtime_error("Can't initialize glfw library");
    }
    glfwSetErrorCallback(fontograph::errorCallback);

    auto fontManager = fontograph::FontManager(optsResult["storage"].as<std::string>());

    std::unique_ptr<fontograph::Drawer> drawer;
    if (optsResult.contains("no-flexible-sizes")) {
        drawer = std::make_unique<fontograph::Drawer>(
            optsResult["window-width"].as<uint32_t>(),
            optsResult["window-height"].as<uint32_t>(),
            fontManager.loadFont(optsResult["font"].as<std::string>()),
            fontManager.loadShaper(optsResult["font"].as<std::string>()));
    } else {
        drawer = std::make_unique<fontograph::Drawer>(
            fontManager.loadFont(optsResult["font"].as<std::string>()),
            fontManager.loadShaper(optsResult["font"].as<std::string>()));
    }

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    drawer->drawPng(
        convert.from_bytes(optsResult["text"].as<std::string>()),
        fontograph::fillRenderingParams(optsResult),
        optsResult["output"].as<std::string>());

    glfwTerminate();
    return 0;
} catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
} catch (...) {
    std::cerr << "Error: unknown error\n";
    return 1;
}
