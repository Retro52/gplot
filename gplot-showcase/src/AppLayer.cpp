#include <include/AppLayer.hpp>
#include <include/AppUtils.hpp>

#include <Core/DriveIO.hpp>
#include <Core/EventsHandler.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <memory>
#include <vector>
#include <cmath>
#include <cstdlib> // for rand()
#include <glm/glm.hpp>

using namespace gplot;
using namespace gplot::core;

void AppLayer::OnAttach()
{
    m_window = std::make_shared<gplot::core::Window>(1200, 800, "Some window");
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    gplot::core::EventsHandler::Initialize(m_window);

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
    ImGui_ImplGlfw_InitForOpenGL(m_window->GetNative(), true);
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

    m_camera = gplot::core::Camera(45.0F);
    m_framebuffer = std::make_unique<gplot::graphics::FBO>();
    m_surface_shader = std::make_unique<gplot::graphics::Shader>("3d", surf_vert.data(), surf_frag.data());
    m_lines_shader = std::make_unique<gplot::graphics::Shader>("lines", line_vert.data(), line_frag.data(), line_geom.data());

    auto [gvertices, gindices] = GenerateGrid(glm::vec3(-100, 0, -100), 40, 40, 5.0F, 0.1F);
    m_grid = std::make_unique<gplot::graphics::Lines>(gvertices, gindices);
    m_framebuffer->SetTexture(std::make_shared<gplot::graphics::Texture>(gplot::graphics::Texture::texsize(m_window->GetWidth(), m_window->GetHeight())));
}

void AppLayer::OnDetach()
{

}

bool AppLayer::OnUpdate()
{
    static bool render_grid = true;
    static int width = m_framebuffer->GetWidth(), height = m_framebuffer->GetHeight();

    gplot::core::EventsHandler::PullEvents();
    const auto dt = gplot::core::EventsHandler::world_dt;

    /* PerspectiveCamera world orientation */
    if (gplot::core::EventsHandler::isCursorLocked)
    {
        m_movement_settings.cameraRotation.y += static_cast<float>(- gplot::core::EventsHandler::mouse_dx * m_movement_settings.mouse_sensitivity / m_framebuffer->GetWidth());
        m_movement_settings.cameraRotation.x = glm::clamp(static_cast<float>(m_movement_settings.cameraRotation.x - gplot::core::EventsHandler::mouse_dy * m_movement_settings.mouse_sensitivity / m_framebuffer->GetHeight()),- 89.0f,89.0f);
        m_camera.SetRotation(m_movement_settings.cameraRotation);
    }

    float camera_speed = m_movement_settings.camera_speed_base;
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
        m_camera.m_pos += static_cast<float>(dt) * camera_speed * m_camera.GetFrontVector();
    }
    if (EventsHandler::IsPressed(Key::S))
    {
        m_camera.m_pos -= static_cast<float>(dt) * camera_speed * m_camera.GetFrontVector();
    }
    if (EventsHandler::IsPressed(Key::D) && !EventsHandler::IsPressed(Key::LeftAlt))
    {
        m_camera.m_pos += static_cast<float>(dt) * camera_speed * m_camera.GetRightVector();
    }
    if (EventsHandler::IsPressed(Key::A))
    {
        m_camera.m_pos -= static_cast<float>(dt) * camera_speed * m_camera.GetRightVector();
    }
    if (EventsHandler::IsPressed(Key::Q))
    {
        m_camera.m_pos += static_cast<float>(dt) * camera_speed * m_camera.GetUpVector();
    }
    if (EventsHandler::IsPressed(Key::E))
    {
        m_camera.m_pos -= static_cast<float>(dt) * camera_speed * m_camera.GetUpVector();
    }

    m_camera.m_aspect_ratio = { m_framebuffer->GetWidth(), m_framebuffer->GetHeight() };

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();

    if (width != m_framebuffer->GetWidth() || height != m_framebuffer->GetHeight())
    {
        m_framebuffer->SetTexture(std::make_shared<gplot::graphics::Texture>(gplot::graphics::Texture::texsize(width, height)));
    }

    m_framebuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_camera.m_aspect_ratio = { m_framebuffer->GetWidth(), m_framebuffer->GetHeight() };
    glViewport(0, 0, m_framebuffer->GetWidth(), m_framebuffer->GetHeight());

    m_surface_shader->Use();
    m_surface_shader->Set("uView", m_camera.GetView());
    m_surface_shader->Set("uCameraPos", m_camera.m_pos);
    m_surface_shader->Set("uProjection", m_camera.GetProjection());
    m_surface_shader->Set("uLightCol", m_sun.color);
    m_surface_shader->Set("uLightDir", m_sun.direction);
    m_surface_shader->Set("uLightInt", glm::vec3(m_sun.ambient, m_sun.diffuse, m_sun.specular));

    for (const auto& mesh : m_meshes)
        mesh->Draw();

    if (render_grid)
    {
        m_lines_shader->Use();
        m_lines_shader->Set("uFadeOutMin", m_grid_settings.grid_fade_out_min);
        m_lines_shader->Set("uFadeOutMax", m_grid_settings.grid_fade_out_max);
        m_lines_shader->Set("uGridColor", m_grid_settings.grid_color);
        m_lines_shader->Set("uCameraPos", m_camera.m_pos);
        m_lines_shader->Set("uLineWidth", m_grid_settings.line_width);
        m_lines_shader->Set("uResolution", glm::vec2(m_framebuffer->GetWidth(), m_framebuffer->GetHeight()));

        m_lines_shader->Set("uView", m_camera.GetView());
        m_lines_shader->Set("uProjection", m_camera.GetProjection());

        m_grid->Draw();
    }

    gplot::graphics::FBO::Reset();
    bool vsync = m_window->GetVSync();

    ImGui::Begin("Controls");
    ImGui::Text("FPS: %f", 1.0 / gplot::core::EventsHandler::world_dt);
    ImGui::Text("World dt: %f", gplot::core::EventsHandler::world_dt);
    ImGui::Text("Surface count: %zu", m_meshes.size());
    ImGui::DragFloat("Far Z", &m_camera.m_far_z);
    ImGui::DragFloat("Near Z", &m_camera.m_near_z, 0.001F);

    ImGui::DragFloat("Move speed", &m_movement_settings.camera_speed_base);
    ImGui::DragFloat("Mouse sensitivity", &m_movement_settings.mouse_sensitivity);

    ImGui::DragFloat2("Aspect ratio", glm::value_ptr(m_camera.m_aspect_ratio));
    ImGui::DragFloat3("Camera position", glm::value_ptr(m_camera.m_pos));
    if (ImGui::DragFloat3("Camera rotation", glm::value_ptr(m_movement_settings.cameraRotation), 0.01F))
    {
        m_camera.SetRotation(m_movement_settings.cameraRotation);
    }

    if (ImGui::Checkbox("Enable vsync", &vsync))
    {
        m_window->SetVSync(vsync);
    }

    ImGui::SeparatorText("Grid");
    ImGui::Checkbox("GRID | Enable", &render_grid);
    ImGui::DragFloat("GRID | Line width", &m_grid_settings.line_width);
    ImGui::DragFloat("GRID | Fade out min", &m_grid_settings.grid_fade_out_min);
    ImGui::DragFloat("GRID | Fade out max", &m_grid_settings.grid_fade_out_max);
    ImGui::ColorEdit3("GRID | Grid color", glm::value_ptr(m_grid_settings.grid_color));

    ImGui::SeparatorText("Sun");
    ImGui::DragFloat("Sun | Diffuse", &m_sun.diffuse, 0.01F);
    ImGui::DragFloat("Sun | Ambient", &m_sun.ambient, 0.01F);
    ImGui::DragFloat("Sun | Specular", &m_sun.specular, 0.01F);
    ImGui::ColorEdit3("Sun | Sun color", glm::value_ptr(m_sun.color));
    ImGui::DragFloat3("Sun | Sun direction", glm::value_ptr(m_sun.direction), 0.01F);

    static bool import_popup = false;
    static int dimX = 1000;
    static int dimY = 1000;
    static char filepath[256] = "";
    static bool randomColor = false;
    static bool importError = false;

    if (ImGui::Button("Import Surface"))
    {
        import_popup = true;
    }

    if (import_popup)
    {
        ImGui::OpenPopup("Import Surface");
    }

    if (ImGui::BeginPopupModal("Import Surface", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Filepath", filepath, IM_ARRAYSIZE(filepath));
        ImGui::InputInt("Dimensions X", &dimX);
        ImGui::InputInt("Dimensions Y", &dimY);
        ImGui::Checkbox("Random Colors", &randomColor);

        if (ImGui::Button("Import"))
        {
            std::vector<float> heights;
            if (ReadCSV(filepath, heights, dimX, dimY))
            {
                auto [vertices, indices] = GenerateSurface(dimX, dimY, 100.0f, 100.0f, heights, randomColor);
                m_meshes.emplace_back(std::make_unique<gplot::graphics::Mesh>(vertices, indices));
                import_popup = false;
                importError = false;
                ImGui::CloseCurrentPopup();
            }
            else
            {
                importError = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            import_popup = false;
            importError = false;
            ImGui::CloseCurrentPopup();
        }

        if (importError)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Import failed! Please check the file path and format.");
        }

        ImGui::EndPopup();
    }

    if (ImGui::Button("Remove Surface"))
    {
        if (!m_meshes.empty())
        {
            m_meshes.pop_back();
        }
    }

    ImGui::End();

    ImGui::Begin("Viewport");

    const auto img_size = ImGui::GetContentRegionAvail();
    ImGui::Image(reinterpret_cast<void*>(m_framebuffer->GetColorTexture()->GetTextureId()), img_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    auto& io = ImGui::GetIO();
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow * backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }

    m_window->SwapBuffers();
    return m_window->IsShouldClose();
}
