#version 330 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 8) out;

uniform float thickness = 0.05;

void main()
{
    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;
    vec2 p2 = gl_in[2].gl_Position.xy;

    vec2 directionPrev = normalize(p1 - p0);
    vec2 directionNext = normalize(p2 - p1);

    vec2 perpendicularPrev = vec2(-directionPrev.y, directionPrev.x) * thickness;
    vec2 perpendicularNext = vec2(-directionNext.y, directionNext.x) * thickness;

    // Emit the vertices for the first line segment
    gl_Position = vec4(p1 + perpendicularPrev, 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(p1 - perpendicularPrev, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p1 + perpendicularNext, 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(p1 - perpendicularNext, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p1 + perpendicularNext, 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(p1 - perpendicularNext, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p2 + perpendicularNext, 0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(p2 - perpendicularNext, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
