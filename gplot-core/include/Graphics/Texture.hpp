#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace gplot::graphics
{
    class Texture
    {
    public:

        using texsize = glm::vec<2, int>;

    public:

        explicit Texture(texsize size, const void* pixels = nullptr) noexcept;

        ~Texture() noexcept;

        Texture(Texture&& other) = delete;
        Texture(const Texture& other) = delete;
        Texture& operator=(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;

        [[nodiscard]] texsize GetSize() const;

        [[nodiscard]] GLuint GetTextureId() const noexcept;

    private:

        texsize m_size;
        GLuint m_texture { 0 };
    };
}