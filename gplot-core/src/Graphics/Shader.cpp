#include <iostream>
#include <Graphics/Shader.hpp>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

using namespace gplot::graphics;

namespace
{
    constexpr const char* CACHE_DIR = ".cache/shader";

    bool CheckForErrors(GLuint id, std::string_view type)
    {
        GLint success = 0;
        GLchar info_log[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(id, GL_COMPILE_STATUS, &success);
            if(success == 0)
            {
                glGetShaderInfoLog(id, std::size(info_log), nullptr, info_log);
                std::cerr << __FILE__ << __LINE__ << ". Failed to compile shader. Log: " << info_log << std::endl;
                return false;
            }
        }
        else
        {
            glGetProgramiv(id, GL_LINK_STATUS, &success);
            if(success == 0)
            {
                glGetShaderInfoLog(id, std::size(info_log), nullptr, info_log);
                std::cerr << __FILE__ << __LINE__ << ". Failed to link shader. Log: " << info_log << std::endl;
                return false;
            }
        }

        return true;
    }
}

Shader::Shader(std::string_view name, const char* vertex_src, const char* fragment_src, const char* geometry_src)
{
    bool success = true;
    GLuint vertex_id, fragment_id, geometry_id;

    vertex_id = glCreateShader(GL_VERTEX_SHADER);
    fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_id, 1, &vertex_src, nullptr);
    glCompileShader(vertex_id);

    glShaderSource(fragment_id, 1, &fragment_src, nullptr);
    glCompileShader(fragment_id);

    success &= CheckForErrors(vertex_id, "VERTEX");
    success &= CheckForErrors(fragment_id, "FRAGMENT");

    // shader Program
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_id);
    glAttachShader(m_id, fragment_id);

    if(geometry_src)
    {
        geometry_id = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_id, 1, &geometry_src, nullptr);
        glCompileShader(geometry_id);

        success &= CheckForErrors(geometry_id, "GEOMETRY");

        glAttachShader(m_id, geometry_id);
    }
    glLinkProgram(m_id);
    success &= CheckForErrors(m_id, "PROGRAM");

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
    if(geometry_src)
    {
        glDeleteShader(fragment_id);
    }

    if (!success)
    {
        throw std::runtime_error(std::string("Failed to create shader ") + name.data());
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::Use() const
{
    glUseProgram(m_id);
}

void Shader::Set(std::string_view name, UniformType data)
{
    if (!m_uniform_location_cache.contains(name))
    {
        const auto location = glGetUniformLocation(m_id, name.data());
        if (location == -1)
        {
            std::cerr << __FILE__ << __LINE__ << "Failed to locate uniform: " << name << "\n";
        }

        m_uniform_location_cache[name] = location;
    }

    const auto gl_location = m_uniform_location_cache[name];
    std::visit([&](auto&& value)
       {
           using T = std::decay_t<decltype(value)>;
           if constexpr (std::is_same_v<T, int>)
           {
               glUniform1i(gl_location, value);
           }
           else if constexpr (std::is_same_v<T, float>)
           {
               glUniform1f(gl_location, value);
           }
           else if constexpr (std::is_same_v<T, glm::vec2>)
           {
               glUniform2fv(gl_location, 1, glm::value_ptr(value));
           }
           else if constexpr (std::is_same_v<T, glm::vec3>)
           {
               glUniform3fv(gl_location, 1, glm::value_ptr(value));
           }
           else if constexpr (std::is_same_v<T, glm::vec4>)
           {
               glUniform4fv(gl_location, 1, glm::value_ptr(value));
           }
           else if constexpr (std::is_same_v<T, glm::mat2>)
           {
               glUniformMatrix2fv(gl_location, 1, GL_FALSE, glm::value_ptr(value));
           }
           else if constexpr (std::is_same_v<T, glm::mat3>)
           {
               glUniformMatrix3fv(gl_location, 1, GL_FALSE, glm::value_ptr(value));
           }
           else if constexpr (std::is_same_v<T, glm::mat4>)
           {
               glUniformMatrix4fv(gl_location, 1, GL_FALSE, glm::value_ptr(value));
           }
       }, data);

}
