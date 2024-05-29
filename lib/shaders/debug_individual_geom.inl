R"(
#version 330 core

uniform vec2 windowSizes;
uniform float scale;

uniform float advance;
uniform uint margin;
uniform float height;
uniform float fontSize;

layout (points) in;
layout (line_strip, max_vertices = 24) out;

in VS_OUT {
    vec2 size;
    vec2 bearing;
} gs_in[];

out vec4 color;

const vec4 outerBoxColor = vec4(vec3(34, 139, 34) / 255.0, 0.4);
const vec4 innerBoxColor = vec4(vec3(255, 20, 147) / 255.0, 0.4);

const vec4 originColor = vec4(vec3(255, 79, 24) / 255.0, 1);
const vec4 dashColor = vec4(originColor.rgb, 0.5);
const vec4 indicatorColor = vec4(innerBoxColor.rgb, 1);
const vec4 indicatorLineColor = vec4(vec3(20, 169, 114) / 255.0, 0.6);

vec2 shrinkToScreen(vec2 vec) {
    return 2 * vec2(vec.x / windowSizes.x, -vec.y / windowSizes.y);
}

vec2 relativeStick(float dy) {
    return scale * shrinkToScreen(vec2(0.0, dy));
}

vec2 adamara(vec2 u, vec2 v) {
    return vec2(u.x * v.x, u.y * v.y);
}

void buildBorders() {
    vec2 relativeSize = scale * shrinkToScreen(gs_in[0].size);
    vec2 relativeBearing = scale * shrinkToScreen(gs_in[0].bearing);
    vec2 relativeMarginVec = scale * shrinkToScreen(vec2(float(margin)));

    vec2 origin = gl_in[0].gl_Position.xy;
    vec2 center =
        origin
        + vec2(relativeBearing.x, relativeBearing.y - relativeSize.y)
        + relativeSize / 2.0;

    gl_Position = vec4(vec3(0.0), 1.0);

    vec2 diagNoMargin = relativeSize / 2.0;
    vec2 diag = diagNoMargin + relativeMarginVec;

    color = outerBoxColor;
    gl_Position.xy = center + adamara(vec2(-1, -1), diag);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(-1, 1), diag);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(1, 1), diag);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(1, -1), diag);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(-1, -1), diag);
    EmitVertex();
    EndPrimitive();

    color = innerBoxColor;
    gl_Position.xy = center + adamara(vec2(-1, -1), diagNoMargin);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(-1, 1), diagNoMargin);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(1, 1), diagNoMargin);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(1, -1), diagNoMargin);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(-1, -1), diagNoMargin);
    EmitVertex();
    EndPrimitive();
}

void buildSegment(vec2 origin, vec2 relativeWidth) {
    gl_Position = vec4(vec3(0.0), 1.0);
    gl_Position.xy = origin;
    EmitVertex();
    gl_Position.xy = origin + relativeWidth;
    EmitVertex();
    EndPrimitive();
}

void buildSegmentWithSerifs(vec2 origin, vec2 relativeWidth, float serifHeight) {
    vec2 relativeSerifVec = 0.5 * shrinkToScreen(vec2(0.f, serifHeight));

    gl_Position = vec4(vec3(0.0), 1.0);
    gl_Position.xy = origin + relativeSerifVec;
    EmitVertex();
    gl_Position.xy = origin - relativeSerifVec;
    EmitVertex();
    EndPrimitive();

    gl_Position.xy = origin;
    EmitVertex();
    gl_Position.xy = origin + relativeWidth;
    EmitVertex();
    EndPrimitive();

    gl_Position.xy = origin + relativeWidth + relativeSerifVec;
    EmitVertex();
    gl_Position.xy = origin + relativeWidth - relativeSerifVec;
    EmitVertex();
    EndPrimitive();
}

const float serif = 2.f;
const float serifPercentage = 0.1;

void buildLines() {
    vec2 origin = gl_in[0].gl_Position.xy;

    vec2 relativeSerifVec = relativeStick(serif);
    vec2 relativeBearing = scale * shrinkToScreen(gs_in[0].bearing);
    vec2 relativeSize = scale * shrinkToScreen(gs_in[0].size);
    vec2 relativeAdvance = scale * shrinkToScreen(vec2(advance, 0.0));

    gl_Position = vec4(vec3(0.0), 1.0);

    color = originColor;
    buildSegment(origin + relativeSerifVec, -2 * relativeSerifVec);

    color = dashColor;
    buildSegment(origin, -relativeStick(height));
    buildSegment(origin + relativeAdvance, -relativeStick(height));

    color = indicatorLineColor;
    buildSegment(
        origin - relativeStick(height),
        relativeAdvance
    );

    color = indicatorColor;
    buildSegmentWithSerifs(
        origin - relativeStick(height) + vec2(relativeBearing.x, 0.0),
        vec2(relativeSize.x, 0.0),
        fontSize * serifPercentage);
}

void main() {
    buildBorders();
    buildLines();
}
)"
