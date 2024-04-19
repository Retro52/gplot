#version 330 core

layout (location = 0) in vec2 Point;
//layout (location = 1) in vec4 aColor;
layout (location = 1) in uint aColor;

uniform mat4 ProjMtx;
uniform vec2 M;
uniform vec2 PltMin;
uniform vec2 PixMin;

out vec4 VertColor;

void main()
{
//    VertColor = aColor;
    VertColor.r = float((aColor >> 0 ) & 0xFFu) / 255.0F;
    VertColor.g = float((aColor >> 8 ) & 0xFFu) / 255.0F;
    VertColor.b = float((aColor >> 16) & 0xFFu) / 255.0F;
    VertColor.a = float((aColor >> 24) & 0xFFu) / 255.0F;

    gl_Position = vec4(Point, 0.0, 1.0);
}
