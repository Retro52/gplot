#include <Graphics/VertexBuffer.hpp>

using namespace gplot::graphics;

VertexBuffer::VertexBuffer(const GeometryBufferDescriptor& descriptor)
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    uint32_t total_size = 0;
    for (const auto& element : descriptor.attributes)
    {
        total_size += std::visit([](auto&& arg) { return arg.size; }, element.type);
    }

    uint32_t index = 0;
    uint32_t offset = 0;
    for (const auto& element : descriptor.attributes)
    {
        std::visit([&](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Float> || std::is_same_v<T, Vec2f> || std::is_same_v<T, Vec3f> || std::is_same_v<T, Vec4f>)
            {
                glVertexAttribPointer(index, GeometryBufferAttributeDescriptor::GetSizeOfShaderDataType(arg), arg.glType, GL_FALSE, total_size, (const void*)offset);
            }
            else
            {
                glVertexAttribIPointer(index, GeometryBufferAttributeDescriptor::GetSizeOfShaderDataType(arg), arg.glType, total_size, (const void*)offset);
            }
            glEnableVertexAttribArray(index);

            index++;
            offset += arg.size;
        }, element.type);
    }

    Unbind();
}

VertexBuffer::~VertexBuffer() noexcept
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void VertexBuffer::Bind() const
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

void VertexBuffer::Unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Update(size_t size, void* data) const
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
}
