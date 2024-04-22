#pragma once

#include <Graphics/Shader.hpp>
#include <Graphics/VertexBuffer.hpp>

namespace gplot
{
    class Plotter
    {
    public:

        struct Vertex
        {
            glm::vec2 pos { 0.0F };
        };

        struct Color
        {
            std::uint32_t color { 0 };
        };

        struct PointSpace
        {
            glm::vec2 min;
            glm::vec2 max;
        };

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

    public:

        explicit Plotter();

        void PlotLines(const std::vector<std::vector<Vertex>>& lines, const std::vector<glm::vec4>& colors, CameraViewport space, float line_thickness = 0.05F);

    private:

        static gplot::graphics::Shader LoadShader();

        static gplot::graphics::VertexBuffer CreateVertexBuffer();

    private:

        CameraViewport m_viewport;

        gplot::graphics::Shader m_shader;

        gplot::graphics::VertexBuffer m_buffer;

    };
}