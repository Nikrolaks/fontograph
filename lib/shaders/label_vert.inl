R"(
#version 330 core

uniform vec2 windowSizes;
uniform float scale;
uniform vec2 size;
uniform vec2 bearing;
uniform float offset;
uniform vec2 start;

out VS_OUT {
    vec2 size;
    vec2 bearing;
} vs_out;

void main()
{
    gl_Position = vec4(start.x + scale * 2 * offset / windowSizes.x, -start.y, 0.0, 1.0);
    vs_out.size = size;
    vs_out.bearing = bearing;
}
)"
