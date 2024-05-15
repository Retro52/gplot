#pragma once

#include <glad/glad.h>

#include <Core/Layer.hpp>
#include <Core/Camera.hpp>
#include <Core/Window.hpp>

#include <Graphics/Mesh.hpp>
#include <Graphics/Lines.hpp>
#include <Graphics/Shader.hpp>

#include <Graphics/FBO.hpp>
#include <Graphics/Texture.hpp>

#include <include/AppTypes.hpp>
#include <memory>

namespace gplot
{
    class AppLayer
        : public gplot::core::Layer
    {
    public:

        void OnAttach() override;

        void OnDetach() override;

        bool OnUpdate() override;

    private:

        gplot::core::Camera m_camera { 45.0F };

        std::unique_ptr<gplot::graphics::FBO> m_framebuffer;

        DirectLight m_sun;

        GridSettings m_grid_settings;

        MovementSettings m_movement_settings;

        std::shared_ptr<gplot::core::Window> m_window;

        std::unique_ptr<gplot::graphics::Shader> m_lines_shader;

        std::unique_ptr<gplot::graphics::Shader> m_surface_shader;

        std::unique_ptr<gplot::graphics::Lines> m_grid;

        std::vector<std::unique_ptr<gplot::graphics::Mesh>> m_meshes;

    };
}
