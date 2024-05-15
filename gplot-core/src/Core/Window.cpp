#include <Core/Window.hpp>
#include <Core/EventsHandler.hpp>

#include <thread>
#include <iostream>

using namespace gplot::core;

Window::Window(int w, int h, const std::string& name, bool fullScreen)
    : m_width(w)
    , m_height(h)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if (fullScreen)
    {
        m_window = glfwCreateWindow(w, h, name.c_str(), glfwGetPrimaryMonitor(), nullptr);
        glfwSetWindowSize(m_window, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
    }
    else
    {
        m_window = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
    }

    if (m_window == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Failure during GLFW window creation");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);

    UpdateSize();
}

Window::~Window()
{
    glfwTerminate();
}

void Window::SetCursorMode(int mode)
{
    glfwSetInputMode(m_window, GLFW_CURSOR, mode);
}

bool Window::IsShouldClose()
{
    return glfwWindowShouldClose(m_window) != 0;
}

void Window::SetShouldClose(bool should_close)
{
    glfwSetWindowShouldClose(m_window, should_close);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_window);
}

void Window::Tick()
{
    /* close window */
    if (EventsHandler::IsJustPressed(GLFW_KEY_ESCAPE))
    {
        Window::SetShouldClose(true);
    }

    /* Show/hide cursor */
    if ((EventsHandler::IsJustPressed(GLFW_KEY_F1) && EventsHandler::IsPressed(GLFW_KEY_LEFT_SHIFT)) || EventsHandler::IsPressed(GLFW_KEY_TAB) && EventsHandler::IsJustPressed(GLFW_KEY_F1))
    {
        EventsHandler::ToggleCursor();
    }
}

int Window::GetWidth() const
{
    return m_width;
}

int Window::GetHeight() const
{
    return m_height;
}

void Window::SetWidth(int w)
{
    if (400 < w && w < 3840)
    {
        m_width = w;
    }
}

void Window::SetHeight(int h)
{
    if (400 < h && h < 2160)
    {
        m_height = h;
    }
}

GLFWwindow* Window::GetNative()
{
    return m_window;
}

bool Window::GetVSync() const
{
    return m_vsync;
}

void Window::SetVSync(bool enable)
{
    m_vsync = enable;
    glfwSwapInterval(enable ? 1 : 0);
}

void Window::UpdateSize()
{
    glfwSetWindowSize(m_window, m_width, m_height);
}
