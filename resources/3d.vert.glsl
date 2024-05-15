#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec3 aColor;

out vec3 vertexPos;
out vec3 vertexNorm;
out vec3 vertexColor;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vertexPos = aPos;
    vertexNorm = aNorm;
    vertexColor = aColor;

    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}
