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

        ~VertexBuffer() noexcept;

        void Bind() const;

        static void Unbind();

        void Update(int id, size_t size, void* data) const;

    private:

        GLuint m_VAO { 0 };

        std::vector<GLuint> m_VBO { 0 };
    };
}
