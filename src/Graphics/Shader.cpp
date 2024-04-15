//
// Created by Anton on 23.08.2022.
//

#include "Shader.h"
#include "../Core/EngineException.h"
#include "../Logging/easylogging++.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader

        if(geometryPath != nullptr)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    shaderPath = { vertexPath, fragmentPath, geometryPath == nullptr ? "" : geometryPath };

    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader

    vertex = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if(geometryPath != nullptr)
    {
        const char * gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    if(geometryPath != nullptr)
    {
        glAttachShader(id, geometry);
    }
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(geometryPath != nullptr)
    {
        glDeleteShader(geometry);
    }

}

void Shader::Use() const
{
    glUseProgram(id);
}

void Shader::setBool(const std::string &name, bool value) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform1i(location, value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }
    glUniform1f(location, value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform2fv(location, 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform2f(location, x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform3fv(location, 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform3f(location, x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform4fv(location, 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniform4f(location, x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    auto location = glGetUniformLocation(id, name.c_str());
   //  if (location == -1 ) { LOG(WARNING) << "Failed to locate shader uniform " + name; }

    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setDirLight(const DirectionalLight& dLight, const glm::vec3& rotation) const
{
    setVec3("dLight.diffuse", dLight.diffuse);
    setVec3("dLight.ambient", dLight.ambient);
    setVec3("dLight.specular", dLight.specular);
    setVec3("dLight.direction", DirectionalLight::GetDirection(rotation));
}

void Shader::setPointLight(int idx, const PointLight& pointLight, const glm::vec3& position) const
{
    std::ostringstream pointLightName;
    pointLightName << "pointLights[" << idx << "].";
    std::string strName = pointLightName.str();

    setVec3(strName + "position", position);
    setFloat(strName + "linear", pointLight.linear);
    setVec3(strName + "ambient", pointLight.ambient);
    setVec3(strName + "diffuse", pointLight.diffuse);
    setVec3(strName + "specular", pointLight.specular);
    setFloat(strName + "constant", pointLight.constant);
    setFloat(strName + "quadratic", pointLight.quadratic);
}


void Shader::checkCompileErrors(GLuint shader, const std::string &type)
{
    GLint success = 0;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(success == 0)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            throw EngineException("Failed to compile shader");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(success == 0)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            throw EngineException("Failed to link shaders");
        }
    }
}