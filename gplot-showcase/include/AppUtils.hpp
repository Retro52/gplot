#pragma once

#include <Graphics/RenderTypes.hpp>

#include <tuple>
#include <vector>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

namespace gplot
{
    std::tuple<std::vector<gplot::graphics::Vertex>, std::vector<GLuint>> GenerateSurface(int numPointsX, int numPointsY, float xRange, float yRange, const std::vector<float>& heights, glm::vec3 color)
    {
        std::vector<gplot::graphics::Vertex> vertices;
        std::vector<GLuint> indices;

        indices.reserve(numPointsX * numPointsY + 1);
        vertices.reserve(numPointsX * numPointsY + 1);

        float xStep = xRange / (numPointsX - 1);
        float yStep = yRange / (numPointsY - 1);

        // Generate vertices with positions and colors
        for (int i = 0; i < numPointsY; ++i)
        {
            for (int j = 0; j < numPointsX; ++j)
            {
                float x = -xRange / 2.0f + j * xStep;
                float z = -yRange / 2.0f + i * yStep;
                float y = heights[i * numPointsY + j];

                vertices.push_back({ glm::vec3(x, y, z), glm::vec3(0.0f), color });
            }
        }

        // Generate indices for the surface grid and compute normals
        for (int i = 0; i < numPointsY - 1; ++i)
        {
            for (int j = 0; j < numPointsX - 1; ++j)
            {
                int topLeft = i * numPointsX + j;
                int topRight = topLeft + 1;
                int bottomLeft = topLeft + numPointsX;
                int bottomRight = bottomLeft + 1;

                // First triangle
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                glm::vec3 edge1 = vertices[bottomLeft].position - vertices[topLeft].position;
                glm::vec3 edge2 = vertices[topRight].position - vertices[topLeft].position;
                glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

                vertices[topLeft].normal += normal;
                vertices[bottomLeft].normal += normal;
                vertices[topRight].normal += normal;

                // Second triangle
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);

                edge1 = vertices[bottomLeft].position - vertices[topRight].position;
                edge2 = vertices[bottomRight].position - vertices[topRight].position;
                normal = glm::normalize(glm::cross(edge1, edge2));

                vertices[topRight].normal += normal;
                vertices[bottomLeft].normal += normal;
                vertices[bottomRight].normal += normal;
            }
        }

        // Normalize the normals
        for (auto& vertex : vertices)
        {
            vertex.normal = glm::normalize(vertex.normal);
        }

        return { vertices, indices };
    }

    bool ReadCSV(const std::string& filepath, std::vector<float>& heights, int numPointsX, int numPointsY)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
            return false;

        try
        {
            std::string line;
            while (std::getline(file, line))
            {
                std::stringstream ss(line);
                std::string value;
                while (std::getline(ss, value, ','))
                {
                    heights.push_back(std::stof(value));
                }
            }
        }
        catch (std::exception& e)
        {
            return false;
        }

        if (heights.size() < numPointsY * numPointsX)
        {
            return false;
        }

        return true;
    }
}


std::tuple<std::vector<gplot::graphics::Line>, std::vector<GLuint>> GenerateGrid(glm::vec3 start, int cols, int rows, float delta, float width)
{
    const auto cpy = start;
    std::vector<GLuint> indices;
    std::vector<gplot::graphics::Line> vertices;

    GLuint ind = 0;
    constexpr GLuint restart = 0xFFFFFFFF;
    for (int i = 0; i < cols; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            vertices.push_back({glm::vec3(start.x, start.y, start.z)});
            vertices.push_back({glm::vec3(start.x+delta, start.y, start.z)});
            vertices.push_back({glm::vec3(start.x+delta, start.y, start.z+delta)});
            vertices.push_back({glm::vec3(start.x, start.y, start.z+delta)});
            vertices.push_back({glm::vec3(start.x, start.y, start.z)});

            indices.push_back(ind);
            indices.push_back(ind+1);
            indices.push_back(ind+1);
            indices.push_back(ind+2);
            indices.push_back(ind+2);
            indices.push_back(ind+3);
            indices.push_back(ind+3);
            indices.push_back(ind);
            ind += 8;

            indices.push_back(restart);
            ind += 1;

            start.x += delta;
        }

        start.z += delta;
        vertices.push_back({glm::vec3(start.x - delta, start.y, start.z)});

        start.x = cpy.x;
        vertices.push_back({glm::vec3(start.x, start.y, start.z)});
    }

    return { vertices, indices };
}
