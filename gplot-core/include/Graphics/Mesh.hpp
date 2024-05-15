#pragma once

#include <Graphics/RenderTypes.hpp>
#include <Graphics/VertexBuffer.hpp>

#include <array>
#include <glad/glad.h>

namespace gplot::graphics
{
    class Mesh
    {
    public:

        Mesh(const std::vector<Vertex>& vertices, const std::vector<GLushort>& indices)
            : Mesh(vertices.data(), vertices.size(), indices.data(), indices.size())
        {
        }

        template<size_t Vertices, size_t Indices>
        Mesh(const std::array<Vertex, Vertices>& vertices, const std::array<GLushort, Indices>& indices)
            : Mesh(vertices.data(), vertices.size(), indices.data(), indices.size())
        {
        }

        void Draw();

    private:

        Mesh(const Vertex* vert_data, size_t vert_size, const GLushort* ind_data, size_t ind_size)
            : m_count(ind_size)
            , m_buffer(GetDescriptor())
        {
            m_buffer.Resize<Vertex>(0, gplot::graphics::VertexBuffer::BufferData_t::eVertexData,vert_size);
            m_buffer.Update<Vertex>(0, gplot::graphics::VertexBuffer::BufferData_t::eVertexData, vert_size, vert_data);

            m_buffer.Resize<GLushort>(0, gplot::graphics::VertexBuffer::BufferData_t::eIndicesData,ind_size);
            m_buffer.Update<GLushort>(0, gplot::graphics::VertexBuffer::BufferData_t::eIndicesData,ind_size, ind_data);
        }

        static VertexBuffer::VertexBufferDescriptor GetDescriptor();

        static std::vector<VertexBuffer::GeometryBufferAttributes> GetGeometryAttributes();

    private:

        size_t m_count { 0 };

        VertexBuffer m_buffer;

    };
}
