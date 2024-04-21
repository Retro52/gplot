#include <Graphics/FBO.hpp>

using namespace gplot::graphics;

FBO::FBO() noexcept
{
    glGenFramebuffers(1, &m_id);
}

FBO::~FBO() noexcept
{
    glDeleteFramebuffers(1, &m_id);
}

void FBO::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FBO::SetTexture(GLuint texture)
{
    m_color_texture = texture;
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    constexpr GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FBO::GetColorTexture() const noexcept
{
    return m_color_texture;
}

void FBO::Reset() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_default_fbo_id);
}

void FBO::SetDefaultFramebuffer(GLuint fbo)
{
    s_default_fbo_id = fbo;
}
