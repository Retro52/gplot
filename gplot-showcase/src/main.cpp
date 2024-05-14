#include <Core/Camera.hpp>
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

#include <memory>
#include <random>
#include <numeric>
#include <iostream>

#include <chrono>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
};

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
    const auto window = SDL_CreateWindow("SomeWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    const auto context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_GL_MakeCurrent(window, context);

//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
//    //io.ConfigViewportsNoAutoMerge = true;
//    //io.ConfigViewportsNoTaskBarIcon = true;
//
//    // Setup Dear ImGui style
//    ImGui::StyleColorsDark();
//
//    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//    ImGuiStyle& style = ImGui::GetStyle();
//    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//    {
//        style.WindowRounding = 0.0f;
//        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//    }
//
//    // Setup Platform/Renderer backends
//    ImGui_ImplSDL2_InitForOpenGL(window, context);
//    ImGui_ImplOpenGL3_Init();

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);
    SDL_GL_SetSwapInterval(0);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    gplot::core::DriveIO disk_io;
    const auto vert = disk_io.Read("../resources/3d.vert.glsl");
    const auto frag = disk_io.Read("../resources/3d.frag.glsl");

    gplot::graphics::Shader shader("3d", vert.data(), frag.data());

    gplot::core::Camera camera(glm::radians(45.0F));

    gplot::graphics::VertexBuffer::VertexBufferDescriptor descriptor;
    gplot::graphics::VertexBuffer::GeometryBufferDescriptor vertex_buffer;

    vertex_buffer.buffers.emplace(gplot::graphics::VertexBuffer::BufferData_t::eVertexData);
    vertex_buffer.attributes.resize(2);

    vertex_buffer.attributes[0].data_count = 3;
    vertex_buffer.attributes[0].is_data_normalized = false;
    vertex_buffer.attributes[0].data = gplot::graphics::VertexBuffer::DataType_t::eFloat32;

    vertex_buffer.attributes[1].data_count = 3;
    vertex_buffer.attributes[1].is_data_normalized = false;
    vertex_buffer.attributes[1].data = gplot::graphics::VertexBuffer::DataType_t::eFloat32;

    descriptor.geometry_buffers.push_back(vertex_buffer);

    gplot::graphics::VertexBuffer buffer(descriptor);

    constexpr std::array<Vertex, 36> vertices =
    {
        // Front face
        Vertex{glm::vec3(-0.5F, -0.5F,  0.5F), glm::vec3(1.0F, 0.0F, 0.0F)},
        Vertex{glm::vec3( 0.5F, -0.5F,  0.5F), glm::vec3(0.0F, 1.0F, 0.0F)},
        Vertex{glm::vec3( 0.5F,  0.5F,  0.5F), glm::vec3(0.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3( 0.5F,  0.5F,  0.5F), glm::vec3(0.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3(-0.5F,  0.5F,  0.5F), glm::vec3(1.0F, 1.0F, 0.0F)},
        Vertex{glm::vec3(-0.5F, -0.5F,  0.5F), glm::vec3(1.0F, 0.0F, 0.0F)},

        // Back face
        Vertex{glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(1.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3( 0.5F, -0.5F, -0.5F), glm::vec3(0.0F, 1.0F, 1.0F)},
        Vertex{glm::vec3( 0.5F,  0.5F, -0.5F), glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex{glm::vec3( 0.5F,  0.5F, -0.5F), glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex{glm::vec3(-0.5F,  0.5F, -0.5F), glm::vec3(0.5F, 0.5F, 0.5F)},
        Vertex{glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(1.0F, 0.0F, 1.0F)},

        // Left face
        Vertex{glm::vec3(-0.5F,  0.5F,  0.5F), glm::vec3(0.0F, 1.0F, 1.0F)},
        Vertex{glm::vec3(-0.5F,  0.5F, -0.5F), glm::vec3(0.0F, 1.0F, 0.0F)},
        Vertex{glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(1.0F, 0.0F, 0.0F)},
        Vertex{glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(1.0F, 0.0F, 0.0F)},
        Vertex{glm::vec3(-0.5F, -0.5F,  0.5F), glm::vec3(0.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3(-0.5F,  0.5F,  0.5F), glm::vec3(0.0F, 1.0F, 1.0F)},

        // Right face
        Vertex{glm::vec3(0.5F,  0.5F,  0.5F), glm::vec3(1.0F, 0.0F, 0.0F)},
        Vertex{glm::vec3(0.5F,  0.5F, -0.5F), glm::vec3(0.0F, 1.0F, 0.0F)},
        Vertex{glm::vec3(0.5F, -0.5F, -0.5F), glm::vec3(0.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3(0.5F, -0.5F, -0.5F), glm::vec3(0.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3(0.5F, -0.5F,  0.5F), glm::vec3(1.0F, 1.0F, 0.0F)},
        Vertex{glm::vec3(0.5F,  0.5F,  0.5F), glm::vec3(1.0F, 0.0F, 0.0F)},

        // Top face
        Vertex{glm::vec3(-0.5F,  0.5F, -0.5F), glm::vec3(1.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3( 0.5F,  0.5F, -0.5F), glm::vec3(0.5F, 0.5F, 0.5F)},
        Vertex{glm::vec3( 0.5F,  0.5F,  0.5F), glm::vec3(0.0F, 1.0F, 1.0F)},
        Vertex{glm::vec3( 0.5F,  0.5F,  0.5F), glm::vec3(0.0F, 1.0F, 1.0F)},
        Vertex{glm::vec3(-0.5F,  0.5F,  0.5F), glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex{glm::vec3(-0.5F,  0.5F, -0.5F), glm::vec3(1.0F, 0.0F, 1.0F)},

        // Bottom face
        Vertex{glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(0.5F, 0.5F, 0.5F)},
        Vertex{glm::vec3( 0.5F, -0.5F, -0.5F), glm::vec3(1.0F, 0.0F, 0.0F)},
        Vertex{glm::vec3( 0.5F, -0.5F,  0.5F), glm::vec3(0.0F, 1.0F, 0.0F)},
        Vertex{glm::vec3( 0.5F, -0.5F,  0.5F), glm::vec3(0.0F, 1.0F, 0.0F)},
        Vertex{glm::vec3(-0.5F, -0.5F,  0.5F), glm::vec3(1.0F, 0.0F, 1.0F)},
        Vertex{glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(0.5F, 0.5F, 0.5F)}
    };

    buffer.Resize<Vertex>(0, gplot::graphics::VertexBuffer::BufferData_t::eVertexData,vertices.size());
    buffer.Update<Vertex>(0, gplot::graphics::VertexBuffer::BufferData_t::eVertexData,vertices.size(), vertices.data());

    bool end = false;
    float camera_speed = 10.0f;
    float camera_rot_speed = 100.0f;
    bool rightMouseButtonPressed = false;
    glm::vec3 cameraRotation(0.0f, 0.0F, 0.0f);

    auto last_time = std::chrono::high_resolution_clock::now();
    while (!end)
    {
        auto now = std::chrono::high_resolution_clock::now();
        float delta = std::chrono::duration<float>(now - last_time).count();
        last_time = now;

        SDL_Event event;
        const Uint8* state = SDL_GetKeyboardState(nullptr);

        while(SDL_PollEvent(&event))
        {
//            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                    {
                        end = true;
                    }
                    else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        SDL_GetWindowSize(window, &width, &height);
                        glViewport(0, 0, width, height);
                        camera.m_aspect_ratio = glm::vec2(width, height);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (rightMouseButtonPressed)
                    {
                        cameraRotation.y -= event.motion.xrel * delta * camera_rot_speed;
                        cameraRotation.x -= event.motion.yrel * delta * camera_rot_speed;
                        camera.SetRotation(cameraRotation);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        rightMouseButtonPressed = true;
                        SDL_ShowCursor(SDL_DISABLE); // Hide cursor
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        rightMouseButtonPressed = false;
                        SDL_ShowCursor(SDL_ENABLE); // Show cursor
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                        SDL_WarpMouseInWindow(window, width/2, height/2);
                    }
                    break;
            }
        }

        if (state[SDL_SCANCODE_W])
            camera.m_pos += camera_speed * camera.GetFrontVector() * delta;
        if (state[SDL_SCANCODE_S])
            camera.m_pos -= camera_speed * camera.GetFrontVector() * delta;
        if (state[SDL_SCANCODE_A])
            camera.m_pos -= camera.GetRightVector() * camera_speed * delta;
        if (state[SDL_SCANCODE_D])
            camera.m_pos += camera.GetRightVector() * camera_speed * delta;
        if (state[SDL_SCANCODE_Q])
            camera.m_pos += camera.GetUpVector() * camera_speed * delta;
        if (state[SDL_SCANCODE_E])
            camera.m_pos -= camera.GetUpVector() * camera_speed * delta;

//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplSDL2_NewFrame();
//        ImGui::NewFrame();
//        ImGui::DockSpaceOverViewport();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();
        shader.Set("view", camera.GetView());
        shader.Set("projection", camera.GetProjection());

        buffer.Bind();

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

//        ImGui::Begin("Test");
//        ImGui::DragFloat("Far Z", &camera.m_far_z);
//        ImGui::DragFloat("Near Z", &camera.m_near_z);
//
//        ImGui::DragFloat("Move speed", &camera_speed);
//        ImGui::DragFloat("Rotation speed", &camera_rot_speed);
//
//        ImGui::DragFloat2("Aspect ratio", glm::value_ptr(camera.m_aspect_ratio));
//        ImGui::DragFloat3("Camera position", glm::value_ptr(camera.m_pos));
//        if (ImGui::DragFloat3("Camera rotation", glm::value_ptr(cameraRotation)))
//        {
//            camera.SetRotation(cameraRotation);
//        }
//
//        ImGui::End();
//
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//        // Update and Render additional Platform Windows
//        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
//        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
//        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
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
