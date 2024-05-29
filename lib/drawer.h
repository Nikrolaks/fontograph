#pragma once

#include "font_manager.h"

#include "galogen/gl.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <filesystem>
#include <string>
#include <vector>

namespace fontograph {

struct RenderingParams {
    float fontSize;
    float relativeOriginX, relativeOriginY;
    int debugVerbose;
    bool showGlyphsBitmap;
};

class Drawer {
public:
    Drawer(
        uint32_t width,
        uint32_t height,
        std::shared_ptr<FontStorage> font,
        std::shared_ptr<Shaper> shaper);
    Drawer(std::shared_ptr<FontStorage> font, std::shared_ptr<Shaper> shaper);
    ~Drawer();

    void resize(uint32_t width, uint32_t height);

    void drawPng(
        const std::u16string& text, RenderingParams params, const std::filesystem::path& output);

private:
    struct LabelUniforms {
        GLuint windowSizes;
        GLuint margin;
        GLuint scale;
        GLuint size;
        GLuint bearing;
        GLuint offset;
        GLuint showBitmap;
        GLuint start;

        virtual void fillLocations(GLuint program) {
            windowSizes = glGetUniformLocation(program, "windowSizes");
            margin = glGetUniformLocation(program, "margin");
            scale = glGetUniformLocation(program, "scale");
            size = glGetUniformLocation(program, "size");
            bearing = glGetUniformLocation(program, "bearing");
            offset = glGetUniformLocation(program, "offset");
            showBitmap = glGetUniformLocation(program, "showBitmap");
            start = glGetUniformLocation(program, "start");
        }
    };

    struct DebugUniforms : public LabelUniforms {
        GLuint advance;
        GLuint ascender;
        GLuint capHeight;
        GLuint xHeight;
        GLuint descender;
        GLuint height;
        GLuint fontSize;

        void fillLocations(GLuint program) override {
            LabelUniforms::fillLocations(program);
            advance = glGetUniformLocation(program, "advance");
            ascender = glGetUniformLocation(program, "ascender");
            capHeight = glGetUniformLocation(program, "capHeight");
            xHeight = glGetUniformLocation(program, "xHeight");
            descender = glGetUniformLocation(program, "descender");
            height = glGetUniformLocation(program, "height");
            fontSize = glGetUniformLocation(program, "fontSize");
        }
    };

    void init();

    void drawDebug(
        const std::vector<recycle::ShapeResult>& letters,
        float textWidth,
        const RenderingParams& params);

    LabelUniforms labelLocations_;
    DebugUniforms debugLinesLocations_, debugIndividualLocations_;

    bool flexibleSizes;
    uint32_t width_, height_;

    GLuint drawingProgram_;
    GLuint debugLinesProgram_;
    GLuint debugIndividualProgram_;

    GLuint textureId_, textureIdLocation_;
    GLuint vao_, vaoDebugLines_, vaoDebugIndividual_;
    GLFWwindow* window_;

    std::shared_ptr<FontStorage> font_;
    std::shared_ptr<Shaper> shaper_;
};

} // namespace fontograph
