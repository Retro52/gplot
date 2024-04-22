#pragma once

#include <glm/glm.hpp>

namespace gplot::core
{
    template<typename T = float>
    struct Rect
    {
        glm::vec<2, T> min { std::numeric_limits<T>::max() };
        glm::vec<2, T> max { std::numeric_limits<T>::lowest() };
    };
    using RectF = Rect<float>;

    struct Color
    {
        std::uint32_t color { 0 };
    };

    struct Vertex
    {
        glm::vec2 pos { 0.0F };
    };
}

