R"(
#version 330 core

uniform sampler2D glyph;
uniform bool showBitmap;

vec4 textColor = vec4(0.3125, 0.535, 0.78, 1.0);
vec4 borderColor = vec4(1.0, 1.0, 0.95, 1.0);

const float textBuffer = -0.5;
const float textGamma = 0.2;
const float outlineWidth = 0.25;
const float outlineBufferMax = 0.7;
const float outlineGamma = 0.1;
const float borderRange = outlineBufferMax - textBuffer;

in vec2 texCoord;

layout (location = 0) out vec4 out_color;

vec4 mapkitModel() {
    float dist = 1.0 - 2.0 * texture(glyph, texCoord).r;
    float outlineBuffer = textBuffer + min(outlineWidth, borderRange);
    float alphaText = 1.0 - smoothstep(textBuffer - textGamma, textBuffer + textGamma, dist);
    float alphaOutline = 1.0 - smoothstep(outlineBuffer - outlineGamma, outlineBuffer + outlineGamma, dist);

    return alphaText * textColor + max(alphaOutline - alphaText, 0.0) * borderColor;
}

void main()
{
    if (showBitmap) {
        out_color = vec4(vec3(texture(glyph, texCoord).r), 1.0);
    } else {
        out_color = mapkitModel();
    }
}
)"
