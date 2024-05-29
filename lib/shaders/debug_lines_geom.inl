R"(
#version 330 core

uniform vec2 windowSizes;
uniform float scale;
uniform float offset;

uniform float ascender;
uniform float capHeight;
uniform float xHeight;
uniform float descender;

layout (points) in;
layout (line_strip, max_vertices = 10) out;

out vec4 color;

const vec4 ascenderColor = vec4(vec3(248, 24, 255) / 255.0, 1);
const vec4 capLineColor = vec4(vec3(50, 18, 122) / 255.0, 1);
const vec4 xLineColor = vec4(vec3(209, 119, 10) / 255.0, 1);
const vec4 descenderColor = vec4(vec3(30, 144, 255) / 255.0, 1);

const vec4 baseLineColor = vec4(vec3(65, 74, 76) / 255.0, 1);

vec2 shrinkToScreen(vec2 vec) {
    return 2 * vec2(vec.x / windowSizes.x, -vec.y / windowSizes.y);
}

void buildSegment(vec2 origin, vec2 relativeWidth) {
    gl_Position = vec4(vec3(0.0), 1.0);
    gl_Position.xy = origin;
    EmitVertex();
    gl_Position.xy = origin + relativeWidth;
    EmitVertex();
    EndPrimitive();
}

vec2 relativeStick(float dy) {
    return scale * shrinkToScreen(vec2(0.0, dy));
}

void main() {
    vec2 origin = gl_in[0].gl_Position.xy;

    vec2 relativeOffsetVec = scale * shrinkToScreen(vec2(offset, 0.0));

    color = ascenderColor;
    buildSegment(origin + relativeStick(ascender), relativeOffsetVec);

    color = capLineColor;
    buildSegment(origin + relativeStick(capHeight), relativeOffsetVec);

    color = xLineColor;
    buildSegment(origin + relativeStick(xHeight), relativeOffsetVec);

    color = baseLineColor;
    buildSegment(origin, relativeOffsetVec);

    color = descenderColor;
    buildSegment(origin + relativeStick(descender), relativeOffsetVec);
}
)"
