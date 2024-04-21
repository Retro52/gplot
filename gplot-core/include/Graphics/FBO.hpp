#pragma once

#include <glad/include/glad/glad.h>

namespace gplot::graphics
{
    class FBO
    {
    public:

        explicit FBO() noexcept;

        ~FBO() noexcept;

        void Bind() const;

        void SetTexture(GLuint texture);

        [[nodiscard]] GLuint GetColorTexture() const noexcept;

        static void Reset() noexcept;

        static void SetDefaultFramebuffer(GLuint fbo);

    private:

        GLuint m_id { 0 };
        GLuint m_color_texture { 0 };

        inline static GLuint s_default_fbo_id;

    };
}
