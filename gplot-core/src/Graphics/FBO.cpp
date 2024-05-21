#include <Graphics/FBO.hpp>
#include <iostream>

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

int FBO::GetWidth() const
{
    return m_texture->GetSize().x;
}

int FBO::GetHeight() const
{
    return m_texture->GetSize().y;
}

void FBO::SetTexture(const std::shared_ptr<Texture>& texture)
{
    m_texture = texture;
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetTextureId(), 0);

    constexpr GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    GenerateRenderBufferDepthAttachment(m_texture->GetSize().x, m_texture->GetSize().y);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<Texture> FBO::GetColorTexture() const noexcept
{
    return m_texture;
}

void FBO::Reset() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_default_fbo_id);
}

void FBO::SetDefaultFramebuffer(GLuint fbo)
{
    s_default_fbo_id = fbo;
}

void FBO::GenerateRenderBufferDepthAttachment(int width, int height)
{
    if (m_rbo)
        glDeleteRenderbuffers(1, &m_rbo);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
            case GL_FRAMEBUFFER_UNDEFINED:
                std::cout << "GL_FRAMEBUFFER_UNDEFINED" << "\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << "\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << "\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << "\n";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << "\n";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << "\n";
                break;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
