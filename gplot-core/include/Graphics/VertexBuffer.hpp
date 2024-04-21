#pragma once

#include <array>
#include <vector>
#include <variant>
#include <glad/glad.h>

namespace gplot::graphics
{
    class VertexBuffer
    {
    public:

        enum class DataType_t
        {
            eUndef,

            eInt32,
            eUInt32,
            eFloat32,
        };

        struct GeometryBufferAttributes
        {
            size_t data_count { 1 };
            DataType_t data { DataType_t::eUndef };
            bool is_data_normalized { false };
        };

        struct GeometryBufferDescriptor
        {
            std::vector<GeometryBufferAttributes> attributes;
        };

        struct VertexBufferDescriptor
        {
            std::vector<GeometryBufferDescriptor> geometry_buffers;
        };

    public:

        explicit VertexBuffer(const VertexBufferDescriptor& descriptor);

        VertexBuffer(VertexBuffer&& ) = delete;
        VertexBuffer(const VertexBuffer& ) = delete;
        VertexBuffer& operator=(VertexBuffer&&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        ~VertexBuffer() noexcept;

        void Bind() const;

        static void Unbind();

        void UnmapBuffer(int id) const;

        void* MapBuffer(int id, size_t offset, size_t length, GLbitfield flags) const;

        void Resize(int id, size_t size) const;

        void Update(int id, size_t size, const void* data, int offset = 0) const;

    private:

        GLuint m_VAO { 0 };

        std::vector<GLuint> m_VBO { 0 };
    };
}
