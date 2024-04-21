#include <Core/DriveIO.hpp>
#include <Graphics/FBO.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/VertexBuffer.hpp>

#include <SDL.h>
#include <SDL_main.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/include/imgui.h>
#include <imgui/include/imgui_impl_sdl2.h>
#include <imgui/include/imgui_impl_opengl3.h>

#include <iostream>

struct Vertex
{
    glm::vec2 pos { 0.0F };
};

struct Color
{
    std::uint32_t color { 0 };
};

std::uint32_t VecToInt32(glm::vec4 color)
{
    std::uint32_t res = 0;
    res |= std::uint32_t(color.r * 255.0F) << 24;
    res |= std::uint32_t(color.g * 255.0F) << 16;
    res |= std::uint32_t(color.b * 255.0F) << 8;
    res |= std::uint32_t(color.a * 255.0F) << 0;

    return res;
}

std::vector<Vertex> generate_sin_wave(int points, int start_x, float step, int x_scale, int y_scale)
{
    std::vector<Vertex> res;
    res.reserve(points);

    Vertex vert;
    for (int i = 0; i < points; i++)
    {
        const auto x = step * i;

        vert.pos.x = start_x + (float(x) / x_scale);
        vert.pos.y = glm::sin(x) / y_scale;

        res.push_back(vert);
    }

    return res;
}

std::vector<Vertex> generate_cos_wave(int points, int start_x, float step, int scale, float y_offset)
{
    std::vector<Vertex> res;
    res.reserve(points);

    Vertex vert;
    for (int i = 0; i < points; i++)
    {
        const auto x = step * i;

        vert.pos.x = start_x + (float(x) / scale);
        vert.pos.y = glm::cos(x);

        res.push_back(vert);
    }

    return res;
}

std::vector<Color> generate_color_buffer(const size_t points, const glm::vec4 colors)
{
    std::vector<Color> buff;
    auto icol = VecToInt32(colors);

    buff.resize(points);
    std::fill_n(buff.begin(), points, Color { icol });

    return buff;
}

#include <chrono>

void DrawLines(const std::vector<std::vector<Vertex>>& lines, const std::vector<glm::vec4>& colors, gplot::graphics::VertexBuffer& buffer)
{
    auto start = std::chrono::high_resolution_clock::now();
    static std::vector<Color> col_cache;
    static std::vector<Vertex> vert_cache;

    col_cache.clear();
    vert_cache.clear();

    std::vector<GLint> firsts;
    std::vector<GLsizei> sizes;

    size_t total_size = 0;
    for (const auto& line : lines)
    {
        firsts.push_back(total_size);
        sizes.push_back(line.size());

        total_size += line.size();
    }

//    col_cache.resize(total_size);
//    vert_cache.resize(total_size);

    for (int i = 0; i < firsts.size(); i++)
    {
        auto colorvec = generate_color_buffer(lines[i].size(), colors[i]);
        col_cache.insert(col_cache.end(), colorvec.begin(), colorvec.end());
        vert_cache.insert(vert_cache.end(), lines[i].begin(), lines[i].end());
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << "\n";

    buffer.Bind();
    buffer.Resize(1, sizeof(Color) * total_size);
    buffer.Resize(0, sizeof(Vertex) * total_size);

    buffer.Update(0, sizeof(Vertex) * vert_cache.size(), vert_cache.data());
    buffer.Update(1, sizeof(Color) * col_cache.size(), col_cache.data());

    glMultiDrawArrays(GL_LINE_STRIP_ADJACENCY, firsts.data(), sizes.data(), lines.size());
}

int main(int argc, char* argv[])
{
    gplot::core::DriveIO disk_io;

    auto frag_code = disk_io.Read("../resources/line.frag.glsl");
    auto vert_code = disk_io.Read("../resources/line.vert.glsl");
    auto geom_code = disk_io.Read("../resources/line.geom.glsl");

    const auto window = SDL_CreateWindow("SomeWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    const auto context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_GL_MakeCurrent(window, context);
    gplot::graphics::Shader shader("line", vert_code.data(), frag_code.data(), geom_code.data());

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
    auto* canvas = new gplot::graphics::Texture({width, height});

    framebuffer.SetTexture(canvas->GetTextureId());

    glm::vec4 color { 1.0F, 0.0F, 0.0F, 1.0F };

    gplot::graphics::VertexBuffer::GeometryBufferDescriptor vb_descriptor;

    vb_descriptor.attributes.resize(1);
    vb_descriptor.attributes[0].is_data_normalized = false;
    vb_descriptor.attributes[0].data_count = 2;
    vb_descriptor.attributes[0].data = gplot::graphics::VertexBuffer::DataType_t::eFloat32;

    gplot::graphics::VertexBuffer::GeometryBufferDescriptor col_descriptor;
    col_descriptor.attributes.resize(1);
    col_descriptor.attributes[0].data_count = 1;
    col_descriptor.attributes[0].data = gplot::graphics::VertexBuffer::DataType_t::eUInt32;

    gplot::graphics::VertexBuffer::VertexBufferDescriptor vao_descriptor;
    vao_descriptor.geometry_buffers.push_back(vb_descriptor);
    vao_descriptor.geometry_buffers.push_back(col_descriptor);

    gplot::graphics::VertexBuffer lines_buffer = gplot::graphics::VertexBuffer(vao_descriptor);

    SDL_GL_SetSwapInterval(0);
    constexpr auto lines_count = 10;
//    constexpr auto lines_count = 10'000;
    std::vector<std::vector<Vertex>> lines(lines_count);
    for (int i = 0; i < lines_count; i++)
    {
        lines[i] = generate_sin_wave(100'000, -1, 1.0F, 20, float(i * 10) / lines_count);
    }

    while (true)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                return 0;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                int width, height;
                SDL_GetWindowSize(window, &width, &height);

                glViewport(0, 0, width, height);
                delete canvas;
                canvas = new gplot::graphics::Texture({width, height});
                framebuffer.SetTexture(canvas->GetTextureId());
            }

            if (event.type == SDL_EventType::SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                return 0;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        framebuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT);
        // Render
        shader.Use();
        DrawLines(lines, std::vector<glm::vec4>(lines.size(), color), lines_buffer);

        gplot::graphics::FBO::Reset();
        gplot::graphics::VertexBuffer::Unbind();

        ImGui::Begin("Test");

        static bool _check = true;
        ImGui::Checkbox("DrawMode", &_check);
        glPolygonMode(GL_FRONT, _check ? GL_FILL : GL_LINE);

        ImGui::ColorPicker4("Line color", glm::value_ptr(color));

        static int pts = 1'000'000;
        static float step = 0.00005;
        static int hor_scale = 20;

        bool update = false;
        update |= ImGui::DragInt("Points", &pts, 1000);
        update |= ImGui::DragInt("Hor scale", &hor_scale, 5);
        update |= ImGui::DragFloat("Hor step", &step, 0.00001, 0.0000001F, 1.0F, "%.7f");
        if (update)
        {
//            lines[0] = generate_sin_wave(pts, -1, step, hor_scale);
//            lines[1] = generate_cos_wave(pts, -1, step, hor_scale);
        }

        ImGui::End();

        ImGui::Begin("Viewport");
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
