#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC

#include "glew.h"
#include "glm/glm.hpp"
#include "../Lighting/DirectionalLight.h"
#include "../Lighting/PointLight.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <array>

class Shader {
public:
    Shader(Shader &&) = delete;

    Shader(const Shader &) = delete;

    /**
     * Shader class constructor, which loads, compiles and links shader to the OpenGL program
     * @param vertexPath path to the vertex shader
     * @param fragmentPath path to the fragment shader
     * @param geometryPath path to the geometry shader
     */
    Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);

    ~Shader() {
        glDeleteProgram(id);
    }

    /**
     * Binds this shader
     */
    void Use() const;

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void setDirLight(const DirectionalLight &dirLight, const glm::vec3 &rotation) const;

    void setPointLight(int idx, const PointLight &pointLight, const glm::vec3 &position) const;

    std::array<std::string, 3> GetShaderPath()
    {
        return shaderPath;
    }

private:
    /**
     * Checks if there were any compilation errors
     * @param shader shader to check
     * @param type type of the shader ("VERTEX", "FRAGMENT", "GEOMETRY")
     */
    static void checkCompileErrors(GLuint shader, const std::string& type);

    unsigned int id;
    std::array<std::string, 3> shaderPath;
};
#endif
