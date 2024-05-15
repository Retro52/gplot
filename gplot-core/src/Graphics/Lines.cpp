#include <Graphics/Lines.hpp>

using namespace gplot::graphics;

void Lines::Draw()
{
    m_buffer.Bind();
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_count));
}

VertexBuffer::VertexBufferDescriptor Lines::GetDescriptor()
{
    gplot::graphics::VertexBuffer::GeometryBufferDescriptor vertex_buffer;

    vertex_buffer.buffers.emplace(gplot::graphics::VertexBuffer::BufferData_t::eVertexData);
    vertex_buffer.attributes = GetGeometryAttributes();

    return { { vertex_buffer } };
}

std::vector<VertexBuffer::GeometryBufferAttributes> Lines::GetGeometryAttributes()
{
    std::vector<VertexBuffer::GeometryBufferAttributes> attributes;
    attributes.resize(1);

    attributes[0].data_count = 3;
    attributes[0].is_data_normalized = false;
    attributes[0].data = gplot::graphics::VertexBuffer::DataType_t::eFloat32;

    return attributes;
}
