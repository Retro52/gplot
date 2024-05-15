#include <glad/glad.h>

#include <Core/Window.hpp>
#include <Core/Camera.hpp>
#include <Core/DriveIO.hpp>
#include <Core/EventsHandler.hpp>

#include <Graphics/FBO.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Lines.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/Texture.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <memory>
#include <random>
#include <numeric>
#include <iostream>

#include <vector>
#include <cmath>
#include <cstdlib> // for rand()
#include <glm/glm.hpp>

std::tuple<std::vector<gplot::graphics::Vertex>, std::vector<GLuint>> GenerateSurface(int numPointsX, int numPointsY, float xRange, float yRange)
{
    std::vector<gplot::graphics::Vertex> vertices;
    std::vector<GLuint> indices;

    float xStep = xRange / (numPointsX - 1);
    float yStep = yRange / (numPointsY - 1);

    // Generate vertices with positions and colors
    for (int i = 0; i < numPointsY; ++i)
    {
        for (int j = 0; j < numPointsX; ++j)
        {
            float x = -xRange / 2.0f + j * xStep;
            float z = -yRange / 2.0f + i * yStep;
            float y = std::sin(x) * std::cos(z);

            // Random color
            float r = static_cast<float>(rand()) / RAND_MAX;
            float g = static_cast<float>(rand()) / RAND_MAX;
            float b = static_cast<float>(rand()) / RAND_MAX;

            vertices.push_back({ glm::vec3(x, y, z), glm::vec3(0.0f), glm::vec3(r, g, b) });
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

#if 0
std::tuple<std::vector<gplot::graphics::Line>, std::vector<GLuint>> GenerateGrid(glm::vec3 start, int cols, int rows, float delta, float width)
{
    const auto cpy = start;
    std::vector<GLuint> indices;
    std::vector<gplot::graphics::Line> vertices;

    GLuint ind = 0;
    float half_width = width / 2;
    for (int i = 0; i < cols; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            vertices.push_back({glm::vec3(start.x, start.y, start.z+half_width)});
            vertices.push_back({glm::vec3(start.x, start.y, start.z-half_width)});
            vertices.push_back({glm::vec3(start.x+delta, start.y, start.z-half_width)});
            vertices.push_back({glm::vec3(start.x+delta, start.y, start.z+half_width)});

            vertices.push_back({glm::vec3(start.x+delta+half_width, start.y, start.z)});
            vertices.push_back({glm::vec3(start.x+delta-half_width, start.y, start.z)});
            vertices.push_back({glm::vec3(start.x+delta-half_width, start.y, start.z+delta)});
            vertices.push_back({glm::vec3(start.x+delta+half_width, start.y, start.z+delta)});

            vertices.push_back({glm::vec3(start.x+half_width, start.y, start.z)});
            vertices.push_back({glm::vec3(start.x-half_width, start.y, start.z)});
            vertices.push_back({glm::vec3(start.x-half_width, start.y, start.z+delta)});
            vertices.push_back({glm::vec3(start.x+half_width, start.y, start.z+delta)});

            vertices.push_back({glm::vec3(start.x, start.y, start.z+delta+half_width)});
            vertices.push_back({glm::vec3(start.x, start.y, start.z+delta-half_width)});
            vertices.push_back({glm::vec3(start.x+delta, start.y, start.z+delta-half_width)});
            vertices.push_back({glm::vec3(start.x+delta, start.y, start.z+delta+half_width)});

            for (int k = 0; k < 4; ++k)
            {
                indices.push_back(ind);
                indices.push_back(ind+1);
                indices.push_back(ind+3);
                indices.push_back(ind+1);
                indices.push_back(ind+2);
                indices.push_back(ind+3);
                ind += 4;
            }

            start.x += delta;
        }

        start.x = cpy.x;
        start.z += delta;
    }

    return { vertices, indices };
}
#endif

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

int main(int argc, char* argv[])
{
    std::shared_ptr<gplot::core::Window> window = std::make_shared<gplot::core::Window>(1200, 800, "Some window");
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    gplot::core::EventsHandler::Initialize(window);

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
    ImGui_ImplGlfw_InitForOpenGL(window->GetNative(), true);
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    gplot::core::DriveIO disk_io;
    const auto surf_vert = disk_io.Read("../resources/3d.vert.glsl");
    const auto surf_frag = disk_io.Read("../resources/3d.frag.glsl");

    const auto line_vert = disk_io.Read("../resources/grid.vert.glsl");
    const auto line_frag = disk_io.Read("../resources/grid.frag.glsl");
    const auto line_geom = disk_io.Read("../resources/grid.geom.glsl");

    gplot::graphics::Shader surface_shader("3d", surf_vert.data(), surf_frag.data());
//    gplot::graphics::Shader lines_shader("lines", line_vert.data(), line_frag.data());
    gplot::graphics::Shader lines_shader("lines", line_vert.data(), line_frag.data(), line_geom.data());

    gplot::core::Camera camera(glm::radians(45.0F));

    constexpr int numPointsX = 1000; // Number of points along the x-axis
    constexpr int numPointsY = 1000; // Number of points along the y-axis
    constexpr float xRange = 100.0f; // Range of x values
    constexpr float yRange = 100.0f; // Range of y values

    auto [gvertices, gindices] = GenerateGrid(glm::vec3(-100, 0, -100), 40, 40, 5.0F, 0.1F);
    auto [vertices, indices] = GenerateSurface(numPointsX, numPointsY, xRange, yRange);

    gplot::graphics::Mesh mesh(vertices, indices);
    gplot::graphics::Lines grid(gvertices, gindices);

    float line_width = 150.0f;
    float grid_fade_out_min = 150.0f;
    float grid_fade_out_max = 300.0f;

    float diffuse = 0.4F;
    float ambient = 0.5F;
    float specular = 0.0F;
    glm::vec3 sun_color(1.0F);
    glm::vec3 sun_direction(0.5F);

    glm::vec3 grid_color (0.5F);

    float camera_speed_base = 10.0f;
    float mouse_sensitivity = 100'000.0f;
    glm::vec3 cameraRotation(0.0f, 0.0F, 0.0f);

    gplot::graphics::FBO framebuffer;
    framebuffer.SetTexture(std::make_shared<gplot::graphics::Texture>(gplot::graphics::Texture::texsize(window->GetWidth(), window->GetHeight())));

    int width = window->GetWidth();
    int height = window->GetHeight();

    using namespace gplot::core;
    while (!window->IsShouldClose())
    {
        gplot::core::EventsHandler::PullEvents();
        const auto dt = gplot::core::EventsHandler::world_dt;

        /* PerspectiveCamera world orientation */
        if (gplot::core::EventsHandler::isCursorLocked)
        {
            cameraRotation.x = glm::clamp(static_cast<float>(cameraRotation.x - gplot::core::EventsHandler::mouse_dy * gplot::core::EventsHandler::world_dt * mouse_sensitivity / window->GetHeight()),- 89.0f,89.0f);
            cameraRotation.y += static_cast<float>(- gplot::core::EventsHandler::mouse_dx * gplot::core::EventsHandler::world_dt * mouse_sensitivity / window->GetWidth());
            camera.SetRotation(cameraRotation);
        }

        float camera_speed = camera_speed_base;
        if (EventsHandler::IsPressed(Key::LeftShift))
        {
            camera_speed *= 10;
        }
        else if (EventsHandler::IsPressed(Key::LeftControl))
        {
            camera_speed /= 10;
        }

        if (EventsHandler::IsPressed(Key::W))
        {
            camera.m_pos += static_cast<float>(dt) * camera_speed * camera.GetFrontVector();
        }
        if (EventsHandler::IsPressed(Key::S))
        {
            camera.m_pos -= static_cast<float>(dt) * camera_speed * camera.GetFrontVector();
        }
        if (EventsHandler::IsPressed(Key::D) && !EventsHandler::IsPressed(Key::LeftAlt))
        {
            camera.m_pos += static_cast<float>(dt) * camera_speed * camera.GetRightVector();
        }
        if (EventsHandler::IsPressed(Key::A))
        {
            camera.m_pos -= static_cast<float>(dt) * camera_speed * camera.GetRightVector();
        }
        if (EventsHandler::IsPressed(Key::Q))
        {
            camera.m_pos += static_cast<float>(dt) * camera_speed * camera.GetUpVector();
        }
        if (EventsHandler::IsPressed(Key::E))
        {
            camera.m_pos -= static_cast<float>(dt) * camera_speed * camera.GetUpVector();
        }

        camera.m_aspect_ratio = { window->GetWidth(), window->GetHeight() };

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        if (width != framebuffer.GetWidth() || height != framebuffer.GetHeight())
        {
            framebuffer.SetTexture(std::make_shared<gplot::graphics::Texture>(gplot::graphics::Texture::texsize(width, height)));
        }

        framebuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.m_aspect_ratio = { framebuffer.GetWidth(), framebuffer.GetHeight() };
        glViewport(0, 0, framebuffer.GetWidth(), framebuffer.GetHeight());

        surface_shader.Use();
        surface_shader.Set("uView", camera.GetView());
        surface_shader.Set("uCameraPos", camera.m_pos);
        surface_shader.Set("uProjection", camera.GetProjection());

        surface_shader.Set("uLightCol", sun_color);
        surface_shader.Set("uLightDir", sun_direction);
        surface_shader.Set("uLightInt", glm::vec3(ambient, diffuse, specular));

        mesh.Draw();

        lines_shader.Use();
        lines_shader.Set("uFadeOutMin", grid_fade_out_min);
        lines_shader.Set("uFadeOutMax", grid_fade_out_max);
        lines_shader.Set("uGridColor", grid_color);
        lines_shader.Set("uCameraPos", camera.m_pos);
        lines_shader.Set("uLineWidth", line_width);
        lines_shader.Set("uResolution", glm::vec2(framebuffer.GetWidth(), framebuffer.GetHeight()));

        lines_shader.Set("uView", camera.GetView());
        lines_shader.Set("uProjection", camera.GetProjection());

        grid.Draw();

        gplot::graphics::FBO::Reset();
        bool vsync = window->GetVSync();

        ImGui::Begin("Test");
        ImGui::DragFloat("Far Z", &camera.m_far_z);
        ImGui::DragFloat("Near Z", &camera.m_near_z, 0.001F);

        ImGui::DragFloat("Move speed", &camera_speed_base);
        ImGui::DragFloat("Mouse sensitivity", &mouse_sensitivity);

        ImGui::DragFloat2("Aspect ratio", glm::value_ptr(camera.m_aspect_ratio));
        ImGui::DragFloat3("Camera position", glm::value_ptr(camera.m_pos));
        if (ImGui::DragFloat3("Camera rotation", glm::value_ptr(cameraRotation)))
        {
            camera.SetRotation(cameraRotation);
        }

        if (ImGui::Checkbox("Enable vsync", &vsync))
        {
            window->SetVSync(vsync);
        }

        ImGui::SeparatorText("Grid");
        ImGui::DragFloat("GRID | Line width", &line_width);
        ImGui::DragFloat("GRID | Fade out min", &grid_fade_out_min);
        ImGui::DragFloat("GRID | Fade out max", &grid_fade_out_max);
        ImGui::ColorEdit3("GRID | Grid color", glm::value_ptr(grid_color));

        ImGui::SeparatorText("Sun");
        ImGui::DragFloat("Sun | Diffuse", &diffuse);
        ImGui::DragFloat("Sun | Ambient", &ambient);
        ImGui::DragFloat("Sun | Specular", &specular);
        ImGui::DragFloat3("Sun | Direction", glm::value_ptr(sun_direction));
        ImGui::ColorEdit3("Sun | Light color", glm::value_ptr(sun_color));

        ImGui::End();

        ImGui::Begin("Viewport");

        const auto img_size = ImGui::GetContentRegionAvail();
        ImGui::Image(reinterpret_cast<void*>(framebuffer.GetColorTexture()->GetTextureId()), img_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        width = static_cast<int>(img_size.x);
        height = static_cast<int>(img_size.y);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow * backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }

        window->SwapBuffers();
    }

    return 0;
}
