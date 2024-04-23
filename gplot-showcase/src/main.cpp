#include <Graphics/FBO.hpp>
#include <Graphics/Texture.hpp>
#include <Plotting/Plotting.hpp>

#include <SDL.h>
#include <SDL_main.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/include/imgui.h>
#include <imgui/include/imgui_impl_sdl2.h>
#include <imgui/include/imgui_impl_opengl3.h>

#include <memory>
#include <random>
#include <numeric>
#include <iostream>

struct PointsData
{
    gplot::core::RectF bounds;
    std::vector<gplot::core::Vertex> vertices;
};

std::vector<glm::vec4> GenerateRandomColors(size_t N)
{
    // Vector to store the generated colors
    std::vector<glm::vec4> colors;

    // Random number generator using the Mersenne Twister engine
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // Generate N colors
    for (size_t i = 0; i < N; ++i) {
        glm::vec4 color(dist(rng), dist(rng), dist(rng), dist(rng)); // RGBA values
        colors.push_back(color);
    }

    return colors;
}

PointsData generate_sin_wave(int points, float start_x, float start_y, float step, int x_scale, int y_scale)
{
    if (points < 0)
    {
        return {};
    }

    PointsData res;
    res.vertices.reserve(points);

    gplot::core::Vertex vert;
    for (int i = 0; i < points; i++)
    {
        const auto x = step * i;

        vert.pos.x = start_x + (float(x) / x_scale);
        vert.pos.y = start_y + glm::sin(x) / y_scale;

        res.bounds.min.x = std::min(res.bounds.min.x, vert.pos.x);
        res.bounds.min.y = std::min(res.bounds.min.y, vert.pos.y);
        res.bounds.max.x = std::max(res.bounds.max.x, vert.pos.x);
        res.bounds.max.y = std::max(res.bounds.max.y, vert.pos.y);

        res.vertices.push_back(vert);
    }

    return res;
}

int main(int argc, char* argv[])
{
    const auto window = SDL_CreateWindow("SomeWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    const auto context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_GL_MakeCurrent(window, context);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    gplot::graphics::FBO framebuffer;
    std::unique_ptr<gplot::graphics::Texture> canvas(new gplot::graphics::Texture({width, height}));

    framebuffer.SetTexture(canvas->GetTextureId());

    glm::vec4 color { 1.0F, 0.0F, 0.0F, 1.0F };
    SDL_GL_SetSwapInterval(0);

    int pts = 100;
    float step = 0.2F;
    int hor_scale = 1;
    int vert_scale = 1;
    int lines_count = 10;
    glm::vec4 pos_shift = { 0, 0, 1.0F, 1.0F };

    gplot::Plotter plotter;

    std::vector<std::vector<gplot::core::Vertex>> lines(lines_count);

    gplot::core::RectF rect;
    lines.resize(lines_count);
    auto colors = GenerateRandomColors(lines.size());
    for (int i = 0; i < lines_count; i++)
    {
        auto data = generate_sin_wave(pts, -1.0F, -1.0F + float(i * 10) / lines_count, step, hor_scale, vert_scale);
        lines[i] = data.vertices;

        rect.min.x = std::min(rect.min.x, data.bounds.min.x);
        rect.min.y = std::min(rect.min.y, data.bounds.min.y);
        rect.max.x = std::max(rect.max.x, data.bounds.max.x);
        rect.max.y = std::max(rect.max.y, data.bounds.max.y);
    }

    gplot::CameraViewport viewport, backup;
    viewport.proportions.x = glm::abs(rect.min.x - rect.max.x);
    viewport.proportions.y = glm::abs(rect.min.y - rect.max.y);
    viewport.center.x = glm::abs(rect.min.x - rect.max.x) / 2;
    viewport.center.y = glm::abs(rect.min.y - rect.max.y) / 2;
    backup = viewport;

    float zoom = 1.0F;
    bool dragging = false;
    bool window_hover = false;
    float line_thickness = 0.001F;
    glm::vec2 last_mouse_pos(0);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    while (true)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type) {
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y != 0 && window_hover)
                    {
                        float zoom_factor = (event.wheel.y > 0) ? 1.1f : 0.9f;

                        zoom *= zoom_factor;
                        viewport.AdjustZoom(zoom_factor);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        dragging = true;
                        last_mouse_pos = glm::vec2(event.button.x, event.button.y);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        dragging = false;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (dragging && window_hover)
                    {
                        glm::vec2 cur_mouse_pos(event.motion.x, event.motion.y);
                        glm::vec2 delta = cur_mouse_pos - last_mouse_pos;

                        viewport.PanView(glm::vec2(-delta.x, delta.y) * 0.01F / zoom);
                        last_mouse_pos = cur_mouse_pos;
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDL_KeyCode::SDLK_f)
                    {
                        pos_shift.x  = 0.0F;
                        pos_shift.y  = 0.0F;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                    {
                        return 0;
                    }
                    else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        SDL_GetWindowSize(window, &width, &height);
                        glViewport(0, 0, width, height);

                        canvas = std::make_unique<gplot::graphics::Texture>(gplot::graphics::Texture::texsize(width, height));
                        framebuffer.SetTexture(canvas->GetTextureId());
                    }
                    break;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        framebuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT);

        plotter.PlotLines(lines, colors, rect, viewport, line_thickness);

        gplot::graphics::FBO::Reset();

        ImGui::Begin("Test");

        static bool _check = true;
        ImGui::Checkbox("DrawMode", &_check);
        glPolygonMode(GL_FRONT, _check ? GL_FILL : GL_LINE);

        ImGui::ColorPicker4("Line color", glm::value_ptr(color));

        bool update = false;
        ImGui::Text("Total points: %d", pts * lines_count);
        update |= ImGui::DragInt("Points", &pts, 100);
        update |= ImGui::DragInt("Lines", &lines_count, 1);
        update |= ImGui::DragInt("Hor scale", &hor_scale, 1);
        update |= ImGui::DragInt("Vert scale", &vert_scale, 1);
        update |= ImGui::DragFloat("Hor step", &step, 0.00001, 0.0000001F, 1.0F, "%.7f");
        update |= ImGui::DragFloat("line_thickness", &line_thickness, 0.0001, 0.0000001F, 1.0F, "%.7f");
        if (update && lines_count > 0)
        {
            rect = {};
            lines.resize(lines_count);
            colors = GenerateRandomColors(lines.size());

            for (int i = 0; i < lines_count; i++)
            {
                auto data = generate_sin_wave(pts, -1.0F, -1.0F + float(i * 10) / lines_count, step, hor_scale, vert_scale);
                lines[i] = data.vertices;

                rect.min.x = std::min(rect.min.x, data.bounds.min.x);
                rect.min.y = std::min(rect.min.y, data.bounds.min.y);
                rect.max.x = std::max(rect.max.x, data.bounds.max.x);
                rect.max.y = std::max(rect.max.y, data.bounds.max.y);
            }

            backup = viewport;
            viewport = {};
            viewport.proportions.x = rect.max.x - rect.min.x;
            viewport.proportions.y = rect.max.y - rect.min.y;
            viewport.proportions.x = backup.proportions.x;
            viewport.proportions.y = backup.proportions.y;
            viewport.center.x = (rect.max.x + rect.min.x) / 2;
            viewport.center.y = (rect.max.y + rect.min.y) / 2;
            backup = viewport;
        }
        ImGui::Text("Plot bounds: min { %f, %f }, max { %f, %f }", rect.min.x, rect.min.y, rect.max.x, rect.max.y);
        ImGui::Text("Original vp: center { %f, %f }, proportions { %f, %f }", backup.center.x, backup.center.y, backup.proportions.x, backup.proportions.y);
        ImGui::Text("Viewport vp: center { %f, %f }, proportions { %f, %f }", viewport.center.x, viewport.center.y, viewport.proportions.x, viewport.proportions.y);

        ImGui::End();

        ImGui::Begin("Viewport");
        dragging &= ImGui::IsWindowHovered();
        window_hover = ImGui::IsWindowHovered();
        ImGui::Image((ImTextureID)canvas->GetTextureId(), ImVec2(canvas->GetSize().x, canvas->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
