#version 330 core

out vec4 FragColor;

uniform float uFadeOutMin;
uniform float uFadeOutMax;
uniform vec3 uCameraPos;
uniform vec3 uGridColor = vec3(0.5);

in vec3 fragPos;

void main()
{

    float distance_to = length(uCameraPos - fragPos);

    float clamped = clamp(distance_to, uFadeOutMin, uFadeOutMax);
    float progress = (clamped - (uFadeOutMin)) / (uFadeOutMax - uFadeOutMin);

    FragColor = vec4(uGridColor, 1.0 - progress);
}
