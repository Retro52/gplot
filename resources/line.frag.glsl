#version 330 core

out vec4 FragColor;
in vec4 GeomColor;
in float FragmentDist;

uniform float uFeather = 0.1;

void main()
{
    float clamped = clamp(abs(FragmentDist), 1.0 - uFeather, 1.0);
    float progress = (clamped - (1.0 - uFeather)) / (uFeather);

    FragColor = vec4(GeomColor.rgb, 1.0 - progress);
}
