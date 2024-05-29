R"(
#version 330 core

uniform uint margin;
uniform vec2 windowSizes;
uniform float scale;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec2 size;
    vec2 bearing;
} gs_in[];

out vec2 texCoord;

vec2 shrinkToScreen(vec2 vec) {
    return 2 * vec2(vec.x / windowSizes.x, -vec.y / windowSizes.y);
}

vec2 adamara(vec2 u, vec2 v) {
    return vec2(u.x * v.x, u.y * v.y);
}

void buildGlyph() {
    vec2 relativeSize = scale * shrinkToScreen(gs_in[0].size);
    vec2 relativeBearing = scale * shrinkToScreen(gs_in[0].bearing);
    vec2 relativeMarginVec = scale * shrinkToScreen(vec2(float(margin)));

    vec2 origin = gl_in[0].gl_Position.xy;
    vec2 center =
        origin
        + vec2(relativeBearing.x, relativeBearing.y - relativeSize.y)
        + relativeSize / 2.0;

    gl_Position = vec4(vec3(0.0), 1.0);

    vec2 diag = relativeSize / 2.0 + relativeMarginVec;

    gl_Position.xy = center + adamara(vec2(-1, 1), diag);
    texCoord = vec2(0.0, 0.0);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(-1, -1), diag);
    texCoord = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(1, 1), diag);
    texCoord = vec2(1.0, 0.0);
    EmitVertex();
    gl_Position.xy = center + adamara(vec2(1, -1), diag);
    texCoord = vec2(1.0);
    EmitVertex();
    EndPrimitive();
}

void main() {
    buildGlyph();
}
)"
