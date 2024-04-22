#pragma once

#include <Core/Math.hpp>

namespace gplot
{
    struct CameraViewport
    {
        glm::vec2 center { 0, 0 };
        glm::vec2 proportions { 2, 2 };

        void PanView(glm::vec2 delta)
        {
            center += delta;
        }

        void AdjustZoom(float factor)
        {
            proportions /= factor;
        }
    };
}