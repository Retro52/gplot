#version 330 core
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec2 uResolution;
uniform float uLineWidth;

in vec3 vertexPos[];
out vec3 fragPos;

void main()
{
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;

    // Calculate direction and perpendicular vector
    vec2 direction = normalize(p1.xz - p0.xz);
    vec2 perp = vec2(-direction.y, direction.x);

    // Convert line width to NDC (Normalized Device Coordinates)
    float halfWidth = uLineWidth / uResolution.y;

    gl_Position = p0 + vec4(perp * halfWidth, 0.0, 0.0);
    fragPos = vertexPos[0];
    EmitVertex();

    gl_Position = p0 - vec4(perp * halfWidth, 0.0, 0.0);
    fragPos = vertexPos[0];
    EmitVertex();

    gl_Position = p1 + vec4(perp * halfWidth, 0.0, 0.0);
    fragPos = vertexPos[1];
    EmitVertex();

    gl_Position = p1 - vec4(perp * halfWidth, 0.0, 0.0);
    fragPos = vertexPos[1];
    EmitVertex();

    EndPrimitive();
}
