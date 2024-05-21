#pragma once

#include <glm/vec3.hpp>

namespace gplot::graphics
{
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