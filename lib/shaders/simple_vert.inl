R"(
#version 330 core

uniform vec2 start;

void main()
{
    gl_Position = vec4(start.x, -start.y, 0.0, 1.0);
}
)"
