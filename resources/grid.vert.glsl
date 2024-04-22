#version 330 core

layout (location = 0) in vec2 aPoint;
layout (location = 1) in uint aColor;

out vec4 VertColor;
uniform mat4 uViewMatrix;

void main()
{
    VertColor.r = float((aColor >> 24) & 0xFFu) / 255.0F;
    VertColor.g = float((aColor >> 16) & 0xFFu) / 255.0F;
    VertColor.b = float((aColor >> 8 ) & 0xFFu) / 255.0F;
    VertColor.a = float((aColor >> 0 ) & 0xFFu) / 255.0F;

    gl_Position = uViewMatrix * vec4(aPoint, 0.0, 1.0);
}
