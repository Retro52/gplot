#pragma once

#include <variant>
#include <optional>
#include <string_view>
#include <unordered_map>

#include <glm/glm.hpp>

namespace gplot::graphics
{
    class Shader
    {
    public:

        using UniformType = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

        Shader(Shader&& ) = delete;
        Shader(const Shader& ) = delete;

        Shader(std::string_view name, const char* vertex_src, const char* fragment_src, std::optional<const char*> geometry_src = std::nullopt);

        ~Shader();

        void Use() const;

        void Set(std::string_view name, UniformType data);

    private:

        unsigned int m_id { 0 };

        std::unordered_map<std::string_view, int> m_uniform_location_cache;

    };
}
