#include <Core/DriveIO.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/VertexBuffer.hpp>

#include <SDL.h>
#include <SDL_main.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    color *= 255.0F;
    std::cout << color.r << color.g << color.b << color.a << std::endl;

    std::uint32_t res = 0;
    res |= std::uint32_t(color.r) << 24;
    res |= std::uint32_t(color.g) << 16;
    res |= std::uint32_t(color.b) << 8;
    res |= std::uint32_t(color.a) << 0;

    std::cout << "Color data: " << std::endl;
    std::cout << "\t VertColor.r: " << color.r / 255.0F << "|" << float((res >> 24 ) & 0xFFu) / 255.0F << std::endl;
    std::cout << "\t VertColor.g: " << color.g / 255.0F << "|" << float((res >> 16 ) & 0xFFu) / 255.0F << std::endl;
    std::cout << "\t VertColor.b: " << color.b / 255.0F << "|" << float((res >> 8  ) & 0xFFu) / 255.0F << std::endl;
    std::cout << "\t VertColor.a: " << color.a / 255.0F << "|" << float((res >> 0  ) & 0xFFu) / 255.0F << std::endl;

    return res;
}

std::vector<Vertex> generate_sin_wave(int points, int start_x, float step, int scale)
{
    std::vector<Vertex> res;
    res.reserve(points);

    Vertex vert;
    for (int i = 0; i < points; i++)
    {
        const auto x = step * i;

        vert.pos.x = start_x + (float(x) / scale);
        vert.pos.y = glm::sin(x);

        res.push_back(vert);
        if (i == 0 || i == points - 1)
        {
            res.push_back(vert);
        }
    }

    return res;
}

void generate_color_buffer(const size_t points, const glm::vec4 colors, std::vector<Color>& buff)
{
    auto icol = VecToInt32(colors);

    if (buff.size() < points)
    {
        buff.resize(points);
    }
    std::fill_n(buff.begin(), points, Color { icol });
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

    std::vector<Vertex> points = generate_sin_wave(1'000'000, -1, 0.00005, 20);

    std::vector<Color> colors;
    glm::vec4 color { 1.0F, 0.0F, 0.0F, 1.0F };
    generate_color_buffer(points.size(), color, colors);

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
            }

            if (event.type == SDL_EventType::SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                return 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render
        shader.Use();

        lines_buffer.Bind();
        lines_buffer.Update(0, points.size() * sizeof(Vertex), points.data());
        lines_buffer.Update(1, colors.size() * sizeof(Color), colors.data());

        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, points.size());
        gplot::graphics::VertexBuffer::Unbind();

        ImGui::Begin("Test");

        static bool _check = true;
        ImGui::Checkbox("DrawMode", &_check);
        glPolygonMode(GL_FRONT, _check ? GL_FILL : GL_LINE);
        if (ImGui::ColorPicker4("Line color", glm::value_ptr(color)))
        {
            generate_color_buffer(points.size(), color, colors);
        }

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
