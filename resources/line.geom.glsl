#version 330 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 uViewMatrix;
uniform float uLineThickness = 0.001;

out vec4 GeomColor;
out float FragmentDist;

in vec4 VertColor[];

void main()
{
    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;
    vec2 p2 = gl_in[2].gl_Position.xy;

    vec2 directionPrev = normalize(p1 - p0);
    vec2 directionNext = normalize(p2 - p1);

    float eps = 0.001f;
    vec2 line_thickness_min = (uViewMatrix * vec4(uLineThickness, uLineThickness, 0.0, 0.0)).xy;
    vec2 line_thickness_processed = vec2(max(line_thickness_min.x, eps), max(line_thickness_min.y, eps));

    vec2 perpendicularPrev = vec2(-directionPrev.y, directionPrev.x) * line_thickness_processed;
    vec2 perpendicularNext = vec2(-directionNext.y, directionNext.x) * line_thickness_processed;

    // Emit the vertices for the first line segment
    FragmentDist = 1;
    GeomColor = vec4(VertColor[2].rgb, 0);
    gl_Position = vec4(p1 + perpendicularPrev, 0.0, 1.0);
    EmitVertex();
    FragmentDist = -1;
    GeomColor = vec4(VertColor[2].rgb, 0);
    gl_Position = vec4(p1 - perpendicularPrev, 0.0, 1.0);
    EmitVertex();

    FragmentDist = 1;
    GeomColor = vec4(VertColor[2].rgb, 0);
    gl_Position = vec4(p2 + perpendicularNext, 0.0, 1.0);
    EmitVertex();
    FragmentDist = -1;
    GeomColor = vec4(VertColor[2].rgb, 0);
    gl_Position = vec4(p2 - perpendicularNext, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
