#include <Core/DriveIO.hpp>
#include <Graphics/Shader.hpp>

#include <SDL.h>
#include <SDL_main.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

std::vector<float> generate_sin_wave(int points, int start_x, float step)
{
    std::vector<float> res;
    res.reserve(points * 6);

    for (int i = 0; i < points; i++)
    {
        res.push_back(((start_x + step * i) / 10) - 1);
        res.push_back(glm::sin(start_x + step * i) / 2);
        res.push_back(1.0F);
        res.push_back(1.0F);
        res.push_back(1.0F);
        res.push_back(1.0F);
    }

    return res;
}


void setupSineWaveVAO(GLuint &VAO, GLuint &VBO, const std::vector<float>& vertices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind the VAO
}

int main(int argc, char* argv[])
{
    gplot::core::DriveIO io;

    auto frag_code = io.Read("../resources/line.frag.glsl");
    auto vert_code = io.Read("../resources/line.vert.glsl");
    auto geom_code = io.Read("../resources/line.geom.glsl");

    const auto window = SDL_CreateWindow("SomeWindow", 0, 0, 400, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);

//    gplot::graphics::Shader shader("line", vert_code.data(), frag_code.data());
    gplot::graphics::Shader shader("line", vert_code.data(), frag_code.data(), geom_code.data());

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    shader.Use();

    GLuint VAO, VBO;
    std::vector<float> points = generate_sin_wave(1'000'000, -1, 0.0001);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind the VAO

    SDL_GL_SetSwapInterval(0);

    while (true)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                return 0;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                int width, height;
                SDL_GetWindowSize(window, &width, &height);

                glViewport(0, 0, width, height);
            }

            if (event.type == SDL_EventType::SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                break;
            }

            // Render
            glClear(GL_COLOR_BUFFER_BIT);

            shader.Use();

            glBindVertexArray(VAO);
//            glDrawArrays(GL_LINE_STRIP, 0, points.size() / 6);
            glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, points.size() / 6);
            glBindVertexArray(0);

            SDL_GL_SwapWindow(window);
        }
    }

    return 0;
}
