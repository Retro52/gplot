#include <Graphics/Texture.hpp>

using namespace gplot::graphics;

Texture::Texture(texsize size, const void* pixels) noexcept
    : m_size(size)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_size.x), static_cast<int>(m_size.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() noexcept
{
    glDeleteTextures(1, &m_texture);
}

Texture::texsize Texture::GetSize() const
{
    return m_size;
}

GLuint Texture::GetTextureId() const noexcept
{
    return m_texture;
}
