#pragma once

#include <Graphics/Texture.hpp>

#include <memory>
#include <glad/include/glad/glad.h>

namespace gplot::graphics
{
    class FBO
    {
    public:

        explicit FBO() noexcept;

        ~FBO() noexcept;

        void Bind() const;

        int GetWidth() const;

        int GetHeight() const;

        void SetTexture(const std::shared_ptr<Texture>& texture);

        [[nodiscard]] std::shared_ptr<Texture> GetColorTexture() const noexcept;

        static void Reset() noexcept;

        static void SetDefaultFramebuffer(GLuint fbo);

    private:

        void GenerateRenderBufferDepthAttachment(int width, int height);

    private:

        GLuint m_id { 0 };
        GLuint m_rbo { 0 };
        std::shared_ptr<Texture> m_texture;

        inline static GLuint s_default_fbo_id;

    };
}
