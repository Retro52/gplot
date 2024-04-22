#version 330 core

layout (location = 0) in vec2 aPoint;
layout (location = 1) in uint aColor;

out vec4 VertColor;

//uniform vec4 uPosShift; // x, y -> position offset; z, w -> position scale by respective axes
uniform mat4 uViewMatrix; // x, y -> position offset; z, w -> position scale by respective axes

void main()
{
    VertColor.r = float((aColor >> 24) & 0xFFu) / 255.0F;
    VertColor.g = float((aColor >> 16) & 0xFFu) / 255.0F;
    VertColor.b = float((aColor >> 8 ) & 0xFFu) / 255.0F;
    VertColor.a = float((aColor >> 0 ) & 0xFFu) / 255.0F;

    gl_Position = uViewMatrix * vec4(aPoint, 0.0, 1.0);
//    vec2 translated_point = (aPoint * vec2(uPosShift.z, uPosShift.w)) + vec2(uPosShift.x, uPosShift.y);
//    gl_Position = vec4(translated_point, 0.0, 1.0);
}
