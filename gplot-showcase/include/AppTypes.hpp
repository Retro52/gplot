#pragma once

#include <glm/glm.hpp>

namespace gplot
{
    struct DirectLight
    {
        float ambient { 0.5F };
        float diffuse { 0.5F };
        float specular { 0.0F };
        glm::vec3 color { 1.0F };
        glm::vec3 direction { 0.5F };
    };

    struct GridSettings
    {
        float line_width { 150.0F };
        float grid_fade_out_min { 150.0F };
        float grid_fade_out_max { 300.0F };

        glm::vec3 grid_color = glm::vec3(0.5F);
    };

    struct MovementSettings
    {
        float camera_speed_base { 10.0F };
        float mouse_sensitivity { 100.0F };

        glm::vec3 cameraRotation = glm::vec3(0.0f, 0.0F, 0.0f);
    };
}
