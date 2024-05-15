#pragma once

#include <glm/glm.hpp>

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

        [[nodiscard]] unsigned int GetTextureId() const noexcept;

    private:

        texsize m_size;
        unsigned int m_texture { 0 };
    };
}