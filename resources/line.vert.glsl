#version 330 core

layout (location = 0) in vec2 Point;
layout (location = 1) in vec4 Color;

uniform mat4 ProjMtx;
uniform vec2 M;
uniform vec2 PltMin;
uniform vec2 PixMin;

out vec4 VertColor;

void main()
{
    VertColor = Color;
//    vec2 pix = PixMin + M * (Point - PltMin);
//    gl_Position = vec4(Point, 0.0, 1.0);
    gl_Position = vec4(Point, 0.0, 1.0);
}
