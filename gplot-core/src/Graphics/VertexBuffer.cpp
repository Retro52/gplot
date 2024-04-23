#include <Graphics/VertexBuffer.hpp>
#include <cstdint>

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
}

VertexBuffer::VertexBuffer(const VertexBufferDescriptor& descriptor)
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    uint32_t attrib_index = 0;
    m_VBO.resize(descriptor.geometry_buffers.size());
    for (int i = 0; i < m_VBO.size(); i++)
    {
        auto& VBO = m_VBO[i];
        const auto& desc = descriptor.geometry_buffers[i];

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

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
    for (auto& VBO : m_VBO)
    {
        glDeleteBuffers(1, &VBO);
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

void VertexBuffer::UnmapBuffer(int id) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[id]);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void VertexBuffer::Resize(int id, size_t size) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[id]);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
}

void VertexBuffer::Update(int id, size_t size, const void* data, int offset) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[id]);
    glBufferSubData(GL_ARRAY_BUFFER, offset, static_cast<GLsizeiptr>(size), data);
}

void* VertexBuffer::MapBufferInternal(int id, size_t offset, size_t length, GLbitfield flags) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[id]);
    return glMapBufferRange(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLintptr>(length), flags);
}
