#include <Graphics/Mesh.hpp>

using namespace gplot::graphics;

void Mesh::Draw()
{
    m_buffer.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_count), GL_UNSIGNED_SHORT, nullptr);
}

VertexBuffer::VertexBufferDescriptor Mesh::GetDescriptor()
{
    gplot::graphics::VertexBuffer::GeometryBufferDescriptor vertex_buffer;

    vertex_buffer.buffers.emplace(gplot::graphics::VertexBuffer::BufferData_t::eVertexData);
    vertex_buffer.buffers.emplace(gplot::graphics::VertexBuffer::BufferData_t::eIndicesData);
    vertex_buffer.attributes = GetGeometryAttributes();

    return { { vertex_buffer } };
}

std::vector<VertexBuffer::GeometryBufferAttributes> Mesh::GetGeometryAttributes()
{
    std::vector<VertexBuffer::GeometryBufferAttributes> attributes;
    attributes.resize(2);

    attributes[0].data_count = 3;
    attributes[0].is_data_normalized = false;
    attributes[0].data = gplot::graphics::VertexBuffer::DataType_t::eFloat32;

    attributes[1].data_count = 3;
    attributes[1].is_data_normalized = false;
    attributes[1].data = gplot::graphics::VertexBuffer::DataType_t::eFloat32;

    return attributes;
}
