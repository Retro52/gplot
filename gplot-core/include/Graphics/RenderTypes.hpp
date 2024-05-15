#pragma once

#include <glm/vec3.hpp>

namespace gplot::graphics
{
    enum class Primitive
    {
        eLines,
        eLinesStrip,
        eLinesStripAdjacent,
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

    struct Line
    {
        glm::vec3 position;
    };
}