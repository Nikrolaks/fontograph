#include "drawer.h"

#include "utils.h"

#include <iostream>
#include <numeric>
#include <stdexcept>

namespace fontograph {

namespace {
GLuint createShader(GLenum type, const char* source) {
    GLuint result = glCreateShader(type);
    glShaderSource(result, 1, &source, nullptr);
    glCompileShader(result);
    GLint status;
    glGetShaderiv(result, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint infoLogLength;
        glGetShaderiv(result, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::string infoLog(infoLogLength, '\0');
        glGetShaderInfoLog(result, infoLog.size(), nullptr, infoLog.data());
        throw std::runtime_error("Shader compilation failed: " + infoLog);
    }
    return result;
}

template<typename... Shaders>
GLuint createProgram(Shaders... shaders) {
    GLuint result = glCreateProgram();
    (glAttachShader(result, shaders), ...);
    glLinkProgram(result);

    GLint status;
    glGetProgramiv(result, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLint infoLogLength;
        glGetProgramiv(result, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::string infoLog(infoLogLength, '\0');
        glGetProgramInfoLog(result, infoLog.size(), nullptr, infoLog.data());
        throw std::runtime_error("Program linkage failed: " + infoLog);
    }
    return result;
}

float calculateTextWidth(
    std::shared_ptr<FontStorage> font, const std::vector<recycle::ShapeResult>& letters) {
    return std::accumulate(letters.begin(), letters.end(), 0.f, [&](float z, const auto& letter) {
        return z + font->glyph(letter.glyphId).advance;
    });
}

} // namespace

void Drawer::resize(uint32_t width, uint32_t height) {
    glfwSetWindowSize(window_, width, height);
    width_ = width;
    height_ = height;
    glViewport(0, 0, width_, height_);
}

void Drawer::init() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_OPENGL_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window_ = glfwCreateWindow(width_, height_, "I am drawing fonts!!!", nullptr, nullptr);

    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Can't create glfw window");
    }

    glfwMakeContextCurrent(window_);

    GLuint simpleVertexShader = createShader(
               GL_VERTEX_SHADER,
#include "shaders/simple_vert.inl"
               ),
           labelVertexShader = createShader(
               GL_VERTEX_SHADER,
#include "shaders/label_vert.inl"
               ),
           labelGeometryShader = createShader(
               GL_GEOMETRY_SHADER,
#include "shaders/label_geom.inl"
               ),
           labelFragmentShader = createShader(
               GL_FRAGMENT_SHADER,
#include "shaders/label_frag.inl"
               ),
           debugLinesGeometryShader = createShader(
               GL_GEOMETRY_SHADER,
#include "shaders/debug_lines_geom.inl"
               ),
           debugIndividualGeometryShader = createShader(
               GL_GEOMETRY_SHADER,
#include "shaders/debug_individual_geom.inl"
               ),
           debugFragmentShader = createShader(
               GL_FRAGMENT_SHADER,
#include "shaders/debug_frag.inl"
           );
    drawingProgram_ = createProgram(labelVertexShader, labelGeometryShader, labelFragmentShader);
    debugLinesProgram_ =
        createProgram(simpleVertexShader, debugLinesGeometryShader, debugFragmentShader);
    debugIndividualProgram_ =
        createProgram(labelVertexShader, debugIndividualGeometryShader, debugFragmentShader);

    glViewport(0, 0, width_, height_);

    glClearColor(0.9f, 0.875f, 0.83f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // texture infos
    glUseProgram(drawingProgram_);

    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // buffers infos
    glGenVertexArrays(1, &vao_);
    glGenVertexArrays(1, &vaoDebugLines_);
    glGenVertexArrays(1, &vaoDebugIndividual_);

    // filling locations
    textureIdLocation_ = glGetUniformLocation(drawingProgram_, "glyph");
    labelLocations_.fillLocations(drawingProgram_);
    debugLinesLocations_.fillLocations(debugLinesProgram_);
    debugIndividualLocations_.fillLocations(debugIndividualProgram_);
}

Drawer::Drawer(
    uint32_t width,
    uint32_t height,
    std::shared_ptr<FontStorage> font,
    std::shared_ptr<Shaper> shaper)
    : flexibleSizes(false)
    , width_(width)
    , height_(height)
    , font_(std::move(font))
    , shaper_(std::move(shaper)) {
    init();
}

Drawer::Drawer(std::shared_ptr<FontStorage> font, std::shared_ptr<Shaper> shaper)
    : flexibleSizes(true)
    , width_(1)
    , height_(1)
    , font_(std::move(font))
    , shaper_(std::move(shaper)) {
    init();
}

void Drawer::drawPng(
    const std::u16string& text, RenderingParams params, const std::filesystem::path& output) {
    auto letters = shaper_->shape(text);
    float scale = params.fontSize / font_->capHeight();
    float roxShift = 0.f, royShift = 0.f;

    if (flexibleSizes) {
        REQUIRE(
            params.relativeOriginX < 0.f && params.relativeOriginY < 0.f,
            "Can't draw text with no less then 50\% paddings");
        float textWidth = calculateTextWidth(font_, letters);
        if (params.showGlyphsBitmap) {
            textWidth += 2.5 * font_->margin() * letters.size();
        }
        textWidth *= scale;

        float textHeight = font_->ascender() - font_->descender();
        textHeight *= scale;

        float newWidth = textWidth / (-params.relativeOriginX);
        float newHeight = (textHeight + 2 * font_->descender() * scale) / (-params.relativeOriginY);

        if (params.debugVerbose >= 2) {
            float heightUp = (font_->height() + font_->descender()) * scale;
            float roy = (params.relativeOriginY + 1.f) / 2.f;
            float t = roy + scale * font_->descender() / newHeight;

            newHeight = newHeight + heightUp;
            royShift = 2.f * (scale * (font_->height() / newHeight +
                                       font_->descender() / (newHeight - heightUp)) -
                              t * heightUp / newHeight);
        }

        resize(newWidth, newHeight);
    }

    params.relativeOriginX += roxShift;
    params.relativeOriginY += royShift;

    glUseProgram(drawingProgram_);
    glUniform1i(labelLocations_.showBitmap, params.showGlyphsBitmap);
    glUniform2f(labelLocations_.windowSizes, width_, height_);
    glUniform1ui(labelLocations_.margin, font_->margin());
    glUniform1i(textureIdLocation_, 1);
    glUniform1f(labelLocations_.scale, scale);
    glUniform2f(labelLocations_.start, params.relativeOriginX, params.relativeOriginY);

    glClearColor(0.9f, 0.875f, 0.83f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    float offset = 0;
    for (const auto& letter: letters) {
        uint32_t glyphIndex = letter.glyphId;

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, textureId_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        const auto& glyph = font_->glyph(glyphIndex);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            glyph.width + 2 * font_->margin(),
            glyph.height + 2 * font_->margin(),
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            glyph.texels.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform2f(labelLocations_.size, glyph.width, glyph.height);
        glUniform2f(labelLocations_.bearing, glyph.bearingX, glyph.bearingY);
        glUniform1f(labelLocations_.offset, offset);
        glBindVertexArray(vao_);
        glDrawArrays(GL_POINTS, 0, 1);

        offset += (float)(glyph.advance);

        if (params.showGlyphsBitmap) {
            offset += 2.5 * font_->margin();
        }
    }

    drawDebug(letters, offset, params);

    std::vector<uint8_t> data(width_ * height_ * 4);
    glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    savePng(data.data(), output, width_, height_);
}

void Drawer::drawDebug(
    const std::vector<recycle::ShapeResult>& letters,
    float textWidth,
    const RenderingParams& params) {
    if (params.debugVerbose < 1) {
        return;
    }

    float scale = params.fontSize / font_->capHeight();

    glUseProgram(debugLinesProgram_);
    glUniform2f(debugLinesLocations_.windowSizes, width_, height_);
    glUniform1ui(debugLinesLocations_.margin, font_->margin());
    glUniform1f(debugLinesLocations_.scale, scale);
    glUniform1f(debugLinesLocations_.offset, textWidth);
    glUniform2f(debugLinesLocations_.start, params.relativeOriginX, params.relativeOriginY);

    glUniform1f(debugLinesLocations_.ascender, font_->ascender());
    glUniform1f(debugLinesLocations_.capHeight, font_->capHeight());
    glUniform1f(debugLinesLocations_.xHeight, font_->xHeight());
    glUniform1f(debugLinesLocations_.descender, font_->descender());
    glUniform1f(debugLinesLocations_.height, font_->height());

    glBindVertexArray(vaoDebugLines_);
    glDrawArrays(GL_POINTS, 0, 1);

    if (params.debugVerbose < 2) {
        return;
    }

    glUseProgram(debugIndividualProgram_);
    glBindVertexArray(vaoDebugIndividual_);
    glUniform2f(debugIndividualLocations_.windowSizes, width_, height_);
    glUniform1f(debugIndividualLocations_.scale, scale);
    glUniform1f(debugIndividualLocations_.fontSize, params.fontSize);
    glUniform2f(debugIndividualLocations_.start, params.relativeOriginX, params.relativeOriginY);

    glUniform1ui(debugIndividualLocations_.margin, font_->margin());
    glUniform1f(debugIndividualLocations_.height, font_->height());

    float offset = 0;
    for (const auto& letter: letters) {
        uint32_t glyphIndex = letter.glyphId;
        const auto& glyph = font_->glyph(glyphIndex);

        glUniform2f(debugIndividualLocations_.size, glyph.width, glyph.height);
        glUniform2f(debugIndividualLocations_.bearing, glyph.bearingX, glyph.bearingY);
        glUniform1f(debugIndividualLocations_.advance, glyph.advance);
        glUniform1f(debugIndividualLocations_.offset, offset);
        glDrawArrays(GL_POINTS, 0, 1);

        offset += float(glyph.advance);

        if (params.showGlyphsBitmap) {
            offset += 2.5 * font_->margin();
        }
    }
}

Drawer::~Drawer() {
    glfwDestroyWindow(window_);
}

} // namespace fontograph
