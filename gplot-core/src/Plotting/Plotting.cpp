#include <Core/DriveIO.hpp>
#include <Plotting/Plotting.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace gplot;

namespace
{
    std::uint32_t VecToInt32(glm::vec4 color)
    {
        std::uint32_t res = 0;
        res |= std::uint32_t(color.r * 255.0F) << 24;
        res |= std::uint32_t(color.g * 255.0F) << 16;
        res |= std::uint32_t(color.b * 255.0F) << 8;
        res |= std::uint32_t(color.a * 255.0F) << 0;

        return res;
    }
}

Plotter::Plotter()
    : m_buffer(CreateVertexBuffer())
    , m_grid_buffer(CreateVertexBuffer())
    , m_shader(LoadLineShader())
    , m_grid_shader(LoadGridShader())
{

}

void Plotter::PlotLines(const std::vector<std::vector<gplot::core::Vertex>>& lines, const std::vector<glm::vec4>& colors, core::RectF bounds, CameraViewport camera, float line_thickness)
{
    glm::mat4 view_matrix = glm::ortho(camera.center.x - camera.proportions.x / 2,
                                       camera.center.x + camera.proportions.x / 2,
                                       camera.center.y - camera.proportions.y / 2,
                                       camera.center.y + camera.proportions.y / 2);

    m_grid_shader.Use();
    m_grid_shader.Set("uViewMatrix", view_matrix);
    RenderGrid(bounds, 10, 10);

    m_shader.Use();
    m_shader.Set("uViewMatrix", view_matrix);
    m_shader.Set("uLineThickness", line_thickness);

    PlotLinesInternal(lines, colors, m_buffer);
}

void Plotter::RenderGrid(core::RectF bounds, int count_x, int count_y) const
{
    std::vector<std::vector<gplot::core::Vertex>> lines;
    lines.reserve(count_x + count_y);

    auto step_x = glm::abs(bounds.min.x - bounds.max.x) / static_cast<float>(count_x);
    auto step_y = glm::abs(bounds.min.y - bounds.max.y) / static_cast<float>(count_y);

    std::vector<gplot::core::Vertex> curr;
    for (int i = 0; i < count_x + 1; i++)
    {
        curr.resize(4);
        curr[0] = { glm::vec2(bounds.min.x + step_x * static_cast<float>(i), bounds.min.y) };
        curr[1] = { glm::vec2(bounds.min.x + step_x * static_cast<float>(i), bounds.min.y) };
        curr[2] = { glm::vec2(bounds.min.x + step_x * static_cast<float>(i), bounds.max.y) };
        curr[3] = { glm::vec2(bounds.min.x + step_x * static_cast<float>(i), bounds.max.y) };
        lines.push_back(curr);
    }

    for (int i = 0; i < count_y + 1; i++)
    {
        curr.resize(4);
        curr[0] = { glm::vec2(bounds.min.x, bounds.min.y + step_y * static_cast<float>(i)) };
        curr[1] = { glm::vec2(bounds.min.x, bounds.min.y + step_y * static_cast<float>(i)) };
        curr[2] = { glm::vec2(bounds.max.x, bounds.min.y + step_y * static_cast<float>(i)) };
        curr[3] = { glm::vec2(bounds.max.x, bounds.min.y + step_y * static_cast<float>(i)) };
        lines.push_back(curr);
    }

    glm::vec4 color = { 0.3F, 0.3F, 0.3F, 1.0F };
    PlotLinesInternal(lines, std::vector<glm::vec4>(lines.size(), color), m_grid_buffer);
}

gplot::graphics::Shader Plotter::LoadGridShader()
{
    gplot::core::DriveIO disk_io;

    auto frag_code = disk_io.Read("../resources/grid.frag.glsl");
    auto vert_code = disk_io.Read("../resources/grid.vert.glsl");

    return {"line", vert_code.data(), frag_code.data()};
}

gplot::graphics::Shader Plotter::LoadLineShader()
{
    gplot::core::DriveIO disk_io;

    auto frag_code = disk_io.Read("../resources/line.frag.glsl");
    auto vert_code = disk_io.Read("../resources/line.vert.glsl");
    auto geom_code = disk_io.Read("../resources/line.geom.glsl");

    return {"line", vert_code.data(), frag_code.data(), geom_code.data()};
}

gplot::graphics::VertexBuffer Plotter::CreateVertexBuffer()
{
    gplot::graphics::VertexBuffer::GeometryBufferDescriptor vb_descriptor;

    vb_descriptor.attributes.resize(1);
    vb_descriptor.attributes[0].is_data_normalized = false;
    vb_descriptor.attributes[0].data_count = 2;
    vb_descriptor.attributes[0].data = gplot::graphics::VertexBuffer::DataType_t::eFloat32;

    gplot::graphics::VertexBuffer::GeometryBufferDescriptor col_descriptor;
    col_descriptor.attributes.resize(1);
    col_descriptor.attributes[0].data_count = 1;
    col_descriptor.attributes[0].data = gplot::graphics::VertexBuffer::DataType_t::eUInt32;

    gplot::graphics::VertexBuffer::VertexBufferDescriptor vao_descriptor;
    vao_descriptor.geometry_buffers.push_back(vb_descriptor);
    vao_descriptor.geometry_buffers.push_back(col_descriptor);

    return gplot::graphics::VertexBuffer(vao_descriptor);
}

void Plotter::PlotLinesInternal(const std::vector<std::vector<gplot::core::Vertex>>& lines, const std::vector<glm::vec4>& colors, const gplot::graphics::VertexBuffer& buffer) const
{
    std::vector<GLint> firsts;
    std::vector<GLsizei> sizes;

    size_t total_size = 0;
    for (const auto& line : lines)
    {
        if (line.empty())
        {
            continue;
        }

        firsts.push_back(total_size);
        sizes.push_back(line.size());

        total_size += line.size();
    }

    if (!total_size)
    {
        return;
    }

    buffer.Bind();
    buffer.Resize(1, sizeof(gplot::core::Color) * total_size);
    buffer.Resize(0, sizeof(gplot::core::Vertex) * total_size);

    auto* colors_ptr = buffer.MapBuffer<gplot::core::Color>(1, 0, total_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    auto* vertex_ptr = buffer.MapBuffer<gplot::core::Vertex>(0, 0, total_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    for (int i = 0; i < firsts.size(); i++)
    {
        std::copy(lines[i].begin(), lines[i].end(), vertex_ptr);
        std::fill(colors_ptr, colors_ptr + lines[i].size(), gplot::core::Color {VecToInt32(colors[i]) });

        colors_ptr += lines[i].size();
        vertex_ptr += lines[i].size();
    }

    buffer.UnmapBuffer(0);
    buffer.UnmapBuffer(1);

    glMultiDrawArrays(GL_LINE_STRIP_ADJACENCY, firsts.data(), sizes.data(), lines.size());
    gplot::graphics::VertexBuffer::Unbind();
}
