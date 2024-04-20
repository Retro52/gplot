#include <Core/DriveIO.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/VertexBuffer.hpp>

#include <SDL.h>
#include <SDL_main.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/include/imgui.h>
#include <imgui/include/imgui_impl_sdl2.h>
#include <imgui/include/imgui_impl_opengl3.h>

struct Vertex
{
    glm::vec2 pos { 0.0F };
    std::uint32_t color { 0 };
};

std::vector<Vertex> generate_sin_wave(int points, int start_x, float step, int scale)
{
    std::vector<Vertex> res;
    res.reserve(points);

    Vertex vert;
    std::uint32_t color = 0x00FF00FF;
    for (int i = 0; i < points; i++)
    {
        const auto x = step * i;

        vert.pos.x = start_x + (float(x) / scale);
        vert.pos.y = glm::sin(x);
        vert.color = color;

        res.push_back(vert);
        if (i == 0 || i == points - 1)
        {
            res.push_back(vert);
        }
    }

    return res;
}

int main(int argc, char* argv[])
{
    gplot::core::DriveIO io;

    auto frag_code = io.Read("../resources/line.frag.glsl");
    auto vert_code = io.Read("../resources/line.vert.glsl");
    auto geom_code = io.Read("../resources/line.geom.glsl");

    const auto window = SDL_CreateWindow("SomeWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    const auto context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_GL_MakeCurrent(window, context);
    gplot::graphics::Shader shader("line", vert_code.data(), frag_code.data(), geom_code.data());

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    shader.Use();

    std::vector<Vertex> points = generate_sin_wave(1'000'000, -1, 0.00005, 20);

    gplot::graphics::VertexBuffer::GeometryBufferDescriptor descriptor;
    descriptor.attributes =
    {
        { gplot::graphics::VertexBuffer::Vec2f() },
        { gplot::graphics::VertexBuffer::UInt() },
    };

    constexpr size_t buff_count = 5;

    size_t curr_id = 0;
    gplot::graphics::VertexBuffer lines_buffer = gplot::graphics::VertexBuffer(descriptor);

    SDL_GL_SetSwapInterval(0);

    while (true)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
//            ImGui_ImplSDL2_ProcessEvent(&event);
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
                return 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplSDL2_NewFrame();
//        ImGui::NewFrame();

        // Render
        shader.Use();

        curr_id = (curr_id + 1) % buff_count;
//        glBindVertexArray(VAO);
        lines_buffer.Bind();
        lines_buffer.Update(points.size() * sizeof(Vertex), points.data());

        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, points.size());
//        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, points.size() / 6);
        gplot::graphics::VertexBuffer::Unbind();

//        ImGui::Begin("Test");
//
//        static bool _check = true;
//        ImGui::Checkbox("DrawMode", &_check);
//        glPolygonMode(GL_FRONT, _check ? GL_FILL : GL_LINE);
//
//        ImGui::End();
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//        // Update and Render additional Platform Windows
//        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
//        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
//        if (imio.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//        {
//            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
//            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
//            ImGui::UpdatePlatformWindows();
//            ImGui::RenderPlatformWindowsDefault();
//            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
//        }

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
