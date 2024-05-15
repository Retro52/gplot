#pragma once

#include <memory>
#include <string>
#include <GLFW/glfw3.h>

namespace gplot::core
{
    class Window
    {
    public:

        ~Window();

        Window(int w, int h, const std::string& name, bool fullScreen = false);

        void SetCursorMode(int mode);

        void SetShouldClose(bool should_close);

        bool IsShouldClose();

        void SwapBuffers();

        GLFWwindow * GetNative();

        void SetWidth(int w);

        void SetHeight(int h);

        void Tick();

        [[nodiscard]] int GetWidth() const;

        [[nodiscard]] int GetHeight() const;

        [[nodiscard]] bool GetVSync() const;

        void SetVSync(bool enable);

    private:

        void UpdateSize();

    private:

        GLFWwindow * m_window { nullptr };

        int m_width { 0 };
        int m_height { 0 };

        bool m_vsync { false };
    };
}
