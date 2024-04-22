#pragma once

#include <Graphics/Shader.hpp>
#include <Graphics/VertexBuffer.hpp>
#include <Plotting/PlottingTypes.hpp>

namespace gplot
{
    class Plotter
    {
    public:

        explicit Plotter();

        void PlotLines(const std::vector<std::vector<gplot::core::Vertex>>& lines, const std::vector<glm::vec4>& colors, core::RectF bounds, CameraViewport camera, float line_thickness = 0.05F);

    private:

        void RenderGrid(core::RectF bounds, int count_x, int count_y) const;

        static gplot::graphics::Shader LoadGridShader();

        static gplot::graphics::Shader LoadLineShader();

        static gplot::graphics::VertexBuffer CreateVertexBuffer();

        void PlotLinesInternal(const std::vector<std::vector<gplot::core::Vertex>>& lines, const std::vector<glm::vec4>& colors, const gplot::graphics::VertexBuffer& buffer) const;

    private:

        gplot::graphics::Shader m_shader;

        gplot::graphics::Shader m_grid_shader;

        gplot::graphics::VertexBuffer m_buffer;

        gplot::graphics::VertexBuffer m_grid_buffer;

    };
}