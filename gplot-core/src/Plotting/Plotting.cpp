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
    , m_shader(LoadShader())
{

}

#include <format>
#include <chrono>
#include <iostream>
void Plotter::PlotLines(const std::vector<std::vector<Vertex>>& lines, const std::vector<glm::vec4>& colors, CameraViewport space, float line_thickness)
{
    m_viewport = space;

    std::vector<GLint> firsts;
    std::vector<GLsizei> sizes;

#if 0
    glm::vec2 min = glm::vec2(std::numeric_limits<float>::max());
    glm::vec2 max = glm::vec2(std::numeric_limits<float>::lowest());

    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& line : lines)
    {
        for (const auto& point : line)
        {
            min.x = std::min(min.x, point.pos.x);
            min.y = std::min(min.y, point.pos.y);
            max.x = std::max(max.x, point.pos.x);
            max.y = std::max(max.y, point.pos.y);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::format("\rExecution time: {} | {}, {} | {}, {}", std::chrono::duration_cast<std::chrono::microseconds>(end-start).count(), min.x, min.y, max.x, max.y);
#endif

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

    m_buffer.Bind();
    m_buffer.Resize(1, sizeof(Color) * total_size);
    m_buffer.Resize(0, sizeof(Vertex) * total_size);

    auto* colors_ptr = static_cast<Color*>(m_buffer.MapBuffer(1, 0, sizeof(Color) * total_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
    auto* vertex_ptr = static_cast<Vertex*>(m_buffer.MapBuffer(0, 0, sizeof(Vertex) * total_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

    for (int i = 0; i < firsts.size(); i++)
    {
        std::copy(lines[i].begin(), lines[i].end(), vertex_ptr);
        std::fill(colors_ptr, colors_ptr + lines[i].size(), Color {VecToInt32(colors[i]) });

        colors_ptr += lines[i].size();
        vertex_ptr += lines[i].size();
    }

    glm::mat4 view_matrix = glm::ortho(m_viewport.center.x - m_viewport.proportions.x / 2,
                                       m_viewport.center.x + m_viewport.proportions.x / 2,
                                       m_viewport.center.y - m_viewport.proportions.y / 2,
                                       m_viewport.center.y + m_viewport.proportions.y / 2);

    m_shader.Use();
//    m_shader.Set("uPosShift", position_shift);
    m_shader.Set("uViewMatrix", view_matrix);
    m_shader.Set("uLineThickness", line_thickness);

    m_buffer.UnmapBuffer(0);
    m_buffer.UnmapBuffer(1);

    glMultiDrawArrays(GL_LINE_STRIP_ADJACENCY, firsts.data(), sizes.data(), lines.size());
    gplot::graphics::VertexBuffer::Unbind();
}

gplot::graphics::Shader Plotter::LoadShader()
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
