#include <Graphics/VertexBuffer.hpp>

#include <glad/glad.h>

using namespace gplot::graphics;

namespace
{
    constexpr size_t GetTypeSize(const VertexBuffer::GeometryBufferAttributes& attr)
    {
        size_t pure_size = 0;
        switch (attr.data)
        {
            case gplot::graphics::VertexBuffer::DataType_t::eInt32:
            case gplot::graphics::VertexBuffer::DataType_t::eUInt32:
            case gplot::graphics::VertexBuffer::DataType_t::eFloat32:
                pure_size = 1;
                break;
            default:
                break;
        }

        return pure_size * attr.data_count;
    }

    constexpr size_t GetOffsetStep(const VertexBuffer::GeometryBufferAttributes& attr)
    {
        size_t data_size = 0;
        switch (attr.data)
        {
            case gplot::graphics::VertexBuffer::DataType_t::eInt32:
            case gplot::graphics::VertexBuffer::DataType_t::eUInt32:
            case gplot::graphics::VertexBuffer::DataType_t::eFloat32:
                data_size = sizeof(std::uint32_t);
                break;
            default:
                break;
        }

        return data_size * GetTypeSize(attr);
    }

    constexpr auto EnumToGlType(const VertexBuffer::DataType_t type)
    {
        switch (type)
        {
            case gplot::graphics::VertexBuffer::DataType_t::eInt32:
                return GL_INT;
            case gplot::graphics::VertexBuffer::DataType_t::eUInt32:
                return GL_UNSIGNED_INT;
            case gplot::graphics::VertexBuffer::DataType_t::eFloat32:
                return GL_FLOAT;
            default:
                break;
        }

        return 0;
    }

    constexpr bool IsFloatingType(const VertexBuffer::DataType_t type)
    {
        return EnumToGlType(type) == GL_FLOAT;
    }

    constexpr auto BufferTypeToGl(VertexBuffer::BufferData_t type)
    {
        switch (type)
        {
            case gplot::graphics::VertexBuffer::BufferData_t::eVertexData:
                return GL_ARRAY_BUFFER;
            case gplot::graphics::VertexBuffer::BufferData_t::eIndicesData:
                return GL_ELEMENT_ARRAY_BUFFER;
        }

        return 0;
    }
}

VertexBuffer::VertexBuffer(const VertexBufferDescriptor& descriptor)
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    uint32_t attrib_index = 0;
    m_buffers.resize(descriptor.geometry_buffers.size());
    for (int i = 0; i < m_buffers.size(); i++)
    {
        const auto& desc = descriptor.geometry_buffers[i];

        if (desc.buffers.contains(BufferData_t::eVertexData))
        {
            auto& VBO = m_buffers[i].VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);
        }
        if (desc.buffers.contains(BufferData_t::eIndicesData))
        {
            auto& EBO = m_buffers[i].EBO;
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);
        }

        GLint total_size = 0;
        for (const auto& element : desc.attributes)
        {
            total_size += static_cast<GLint>(GetOffsetStep(element));
        }

        GLuint64 offset = 0;
        for (const auto& element : desc.attributes)
        {
            const auto size = static_cast<GLint>(GetTypeSize(element));
            if (IsFloatingType(element.data))
            {
                const auto norm = element.is_data_normalized ? GL_TRUE : GL_FALSE;
                glVertexAttribPointer(attrib_index, size, EnumToGlType(element.data), norm, total_size, (const void*)offset);
            }
            else
            {
                glVertexAttribIPointer(attrib_index, size, EnumToGlType(element.data), total_size, (const void*)offset);
            }
            glEnableVertexAttribArray(attrib_index);

            attrib_index++;
            offset += GetOffsetStep(element);
        }
    }

    glBindVertexArray(0);
}

VertexBuffer::~VertexBuffer() noexcept
{
    for (auto& buffer : m_buffers)
    {
        if (buffer.VBO)
            glDeleteBuffers(1, &buffer.VBO);
        if (buffer.EBO)
            glDeleteBuffers(1, &buffer.EBO);
    }

    glDeleteVertexArrays(1, &m_VAO);
}

void VertexBuffer::Bind() const
{
    glBindVertexArray(m_VAO);
}

void VertexBuffer::Unbind()
{
    glBindVertexArray(0);
}

void VertexBuffer::UnmapBuffer(int id, BufferData_t buffer) const
{
    const auto type = BufferTypeToGl(buffer);

    glBindBuffer(type, GetBufferId(id, buffer));
    glUnmapBuffer(type);
}

unsigned int VertexBuffer::GetBufferId(int id, BufferData_t buffer) const
{
    switch (buffer)
    {
        case BufferData_t::eVertexData:
            return m_buffers[id].VBO;
        case BufferData_t::eIndicesData:
            return m_buffers[id].EBO;
    }

    return 0;
}

void VertexBuffer::ResizeInternal(int id, BufferData_t buffer, size_t size) const
{
    const auto type = BufferTypeToGl(buffer);

    glBindBuffer(type, GetBufferId(id, buffer));
    glBufferData(type, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
}

void VertexBuffer::UpdateInternal(int id, BufferData_t buffer, size_t size, const void* data, int offset) const
{
    const auto type = BufferTypeToGl(buffer);

    glBindBuffer(type, GetBufferId(id, buffer));
    glBufferSubData(type, offset, static_cast<GLsizeiptr>(size), data);
}

void* VertexBuffer::MapBufferInternal(int id, BufferData_t buffer, size_t offset, size_t length, GLbitfield flags) const
{
    const auto type = BufferTypeToGl(buffer);

    glBindBuffer(type, GetBufferId(id, buffer));
    return glMapBufferRange(type, static_cast<GLintptr>(offset), static_cast<GLintptr>(length), flags);
}
