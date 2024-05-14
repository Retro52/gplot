#pragma once

#include <array>
#include <vector>
#include <variant>
#include <unordered_set>

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

        enum class BufferData_t
        {
            eVertexData,
            eIndicesData
        };

        struct GeometryBufferAttributes
        {
            size_t data_count { 1 };
            DataType_t data { DataType_t::eUndef };
            bool is_data_normalized { false };
        };

        struct GeometryBufferDescriptor
        {
            std::unordered_set<BufferData_t> buffers;
            std::vector<GeometryBufferAttributes> attributes;
        };

        struct VertexBufferDescriptor
        {
            std::vector<GeometryBufferDescriptor> geometry_buffers;
        };

        struct DataBuffer
        {
            GLuint VBO { 0 };
            GLuint EBO { 0 };
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

        void UnmapBuffer(int id, BufferData_t buffer) const;

        template<typename T>
        [[nodiscard]] T* MapBuffer(int id, BufferData_t buffer, size_t offset, size_t size, GLbitfield flags) const
        {
            return static_cast<T*>(MapBufferInternal(id, buffer, offset, size * sizeof(T), flags));
        }

        template<typename T>
        void Resize(int id, BufferData_t buffer, size_t count) const
        {
            return ResizeInternal(id, buffer, count * sizeof(T));
        }

        template<typename T>
        void Update(int id, BufferData_t buffer, size_t count, const T* data, int offset = 0) const
        {
            return UpdateInternal(id, buffer, count * sizeof(T), data);
        }

    private:

        [[nodiscard]] GLuint GetBufferId(int id, BufferData_t buffer) const;

        void ResizeInternal(int id, BufferData_t buffer, size_t size) const;

        void UpdateInternal(int id, BufferData_t buffer, size_t size, const void* data, int offset = 0) const;

        [[nodiscard]] void* MapBufferInternal(int id, BufferData_t buffer, size_t offset, size_t length, GLbitfield flags) const;

    private:

        GLuint m_VAO { 0 };

        std::vector<DataBuffer> m_buffers;
    };
}
