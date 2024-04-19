#pragma once

#include <vector>
#include <variant>
#include <glad/glad.h>

namespace gplot::graphics
{
    class VertexBuffer
    {
    public:

        struct Float { static constexpr GLenum glType = GL_FLOAT; static constexpr std::uint32_t size = 1 * sizeof(float); };
        struct Vec2f { static constexpr GLenum glType = GL_FLOAT; static constexpr std::uint32_t size = 2 * sizeof(float); };
        struct Vec3f { static constexpr GLenum glType = GL_FLOAT; static constexpr std::uint32_t size = 3 * sizeof(float); };
        struct Vec4f { static constexpr GLenum glType = GL_FLOAT; static constexpr std::uint32_t size = 4 * sizeof(float); };

        struct Int { static constexpr GLenum glType = GL_INT;   static constexpr std::uint32_t size = 1 * sizeof(std::int32_t); };
        struct Vec2i { static constexpr GLenum glType = GL_INT; static constexpr std::uint32_t size = 2 * sizeof(std::int32_t); };
        struct Vec3i { static constexpr GLenum glType = GL_INT; static constexpr std::uint32_t size = 3 * sizeof(std::int32_t); };
        struct Vec4i { static constexpr GLenum glType = GL_INT; static constexpr std::uint32_t size = 4 * sizeof(std::int32_t); };

        struct UInt { static constexpr GLenum glType = GL_UNSIGNED_INT;   static constexpr std::uint32_t size = 1 * sizeof(std::uint32_t); };
        struct UVec2i { static constexpr GLenum glType = GL_UNSIGNED_INT; static constexpr std::uint32_t size = 2 * sizeof(std::uint32_t); };
        struct UVec3i { static constexpr GLenum glType = GL_UNSIGNED_INT; static constexpr std::uint32_t size = 3 * sizeof(std::uint32_t); };
        struct UVec4i { static constexpr GLenum glType = GL_UNSIGNED_INT; static constexpr std::uint32_t size = 4 * sizeof(std::uint32_t); };

        using ShaderDataType = std::variant<Float, Vec2f,  Vec3f,  Vec4f,
                                            Int,   Vec2i,  Vec3i,  Vec4i,
                                            UInt,  UVec2i, UVec3i, UVec4i>;

        struct GeometryBufferAttributeDescriptor
        {
            ShaderDataType type;

            constexpr static uint32_t GetSizeOfShaderDataType(const ShaderDataType& type)
            {
                return std::visit([](auto&& arg) -> uint32_t
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, Float>      || std::is_same_v<T, Int>   || std::is_same_v<T, UInt>  ) return 1;
                    else if constexpr (std::is_same_v<T, Vec2f> || std::is_same_v<T, Vec2i> || std::is_same_v<T, UVec2i>) return 2;
                    else if constexpr (std::is_same_v<T, Vec3f> || std::is_same_v<T, Vec3i> || std::is_same_v<T, UVec3i>) return 3;
                    else if constexpr (std::is_same_v<T, Vec4f> || std::is_same_v<T, Vec4i> || std::is_same_v<T, UVec4i>) return 4;
                    else return 0;
                }, type);
            }
        };

        struct GeometryBufferDescriptor
        {
            std::vector<GeometryBufferAttributeDescriptor> attributes;
        };

    public:

        explicit VertexBuffer(const GeometryBufferDescriptor& descriptor);

        ~VertexBuffer() noexcept;

        void Bind() const;

        static void Unbind();

        void Update(size_t size, void* data) const;

    private:

        GLuint m_VAO { 0 };

        GLuint m_VBO { 0 };
    };
}
