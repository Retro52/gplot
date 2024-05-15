#pragma once

#include <Graphics/RenderTypes.hpp>
#include <Graphics/VertexBuffer.hpp>

#include <array>
#include <glad/glad.h>

namespace gplot::graphics
{
    class Lines
    {
    public:

        Lines(const std::vector<Line>& vertices, const std::vector<GLuint>& indices)
            : Lines(vertices.data(), vertices.size(), indices.data(), indices.size())
        {
        }

        template<size_t Vertices, size_t Indices>
        Lines(const std::array<Line, Vertices>& vertices, const std::array<GLuint, Indices>& indices)
            : Lines(vertices.data(), vertices.size(), indices.data(), indices.size())
        {
        }

        void Draw();

    private:

        Lines(const Line* vert_data, size_t vert_size, const GLuint* ind_data, size_t ind_size)
            : m_count(vert_size)
            , m_buffer(GetDescriptor())
        {
            m_buffer.Resize<Line>(0, gplot::graphics::VertexBuffer::BufferData_t::eVertexData, vert_size);
            m_buffer.Update<Line>(0, gplot::graphics::VertexBuffer::BufferData_t::eVertexData, vert_size, vert_data);
        }

        static VertexBuffer::VertexBufferDescriptor GetDescriptor();

        static std::vector<VertexBuffer::GeometryBufferAttributes> GetGeometryAttributes();

    private:

        size_t m_count { 0 };

        VertexBuffer m_buffer;

    };
}
