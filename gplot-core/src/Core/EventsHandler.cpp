#include <glad/glad.h>
#include <Core/EventsHandler.hpp>

using namespace gplot::core;

void EventsHandler::CursorCallback(GLFWwindow* /* window */, double xpos, double ypos)
{
    if (EventsHandler::isCursorStarted)
    {
        EventsHandler::mouse_dx += (float) xpos - EventsHandler::mouse_x;
        EventsHandler::mouse_dy += (float) ypos - EventsHandler::mouse_y;
    }
    else
    {
        EventsHandler::isCursorStarted = true;
    }

    EventsHandler::mouse_x = (float) xpos;
    EventsHandler::mouse_y = (float) ypos;
}

void EventsHandler::MouseCallback(GLFWwindow* /* window */, int button, int action, int /* mode */mode)
{
    if (MOUSE_BUTTONS_OFFSET + button > 1032 || button < 0)
    {
        return;
    }
    if (action == GLFW_PRESS)
    {
        EventsHandler::m_keys_state.at(MOUSE_BUTTONS_OFFSET + button) = true;
        EventsHandler::m_keys_state_frame.at(MOUSE_BUTTONS_OFFSET + button) = EventsHandler::m_curr_frame;
    }
    else if (action == GLFW_RELEASE)
    {
        EventsHandler::m_keys_state.at(MOUSE_BUTTONS_OFFSET + button) = false;
        EventsHandler::m_keys_state_frame.at(MOUSE_BUTTONS_OFFSET + button) = EventsHandler::m_curr_frame;
    }
}

void EventsHandler::KeyCallback(GLFWwindow* /* window */, int key, int /* scancode */ scancode, int action, int /* mode */mode)
{
    if (key > MOUSE_BUTTONS_OFFSET || key < 0)
    {
        return;
    }
    if (action == GLFW_PRESS)
    {
        EventsHandler::m_keys_state.at(key) = true;
        EventsHandler::m_keys_state_frame.at(key) = EventsHandler::m_curr_frame;


    }
    else if (action == GLFW_RELEASE)
    {
        EventsHandler::m_keys_state.at(key) = false;
        EventsHandler::m_keys_state_frame.at(key) = EventsHandler::m_curr_frame;
    }
}

void EventsHandler::WindowCallback(GLFWwindow * /*window*/, int width, int height)
{
    m_window.lock()->SetHeight(height);
    m_window.lock()->SetWidth(width);
}

int EventsHandler::Initialize(const std::shared_ptr<gplot::core::Window>& window)
{
    m_window = window;
    glfwSetKeyCallback(window->GetNative(), KeyCallback);
    glfwSetMouseButtonCallback(window->GetNative(), MouseCallback);
    glfwSetCursorPosCallback(window->GetNative(), CursorCallback);
    glfwSetWindowSizeCallback(window->GetNative(), WindowCallback);
    return 0;
}

bool EventsHandler::IsPressed(int keycode)
{
    if (keycode < 0 || keycode >= MOUSE_BUTTONS_OFFSET)
    {
        return false;
    }
    return m_keys_state.at(keycode);
}

bool EventsHandler::IsJustPressed(int keycode)
{
    if (keycode < 0 || keycode >= MOUSE_BUTTONS_OFFSET)
    {
        return false;
    }
    return m_keys_state.at(keycode) && m_keys_state_frame.at(keycode) == m_curr_frame;
}

bool EventsHandler::IsClicked(int button)
{
    int index = MOUSE_BUTTONS_OFFSET + button;
    return m_keys_state.at(index);
}

bool EventsHandler::IsJustClicked(int button)
{
    int index = MOUSE_BUTTONS_OFFSET + button;
    return m_keys_state.at(index) && m_keys_state_frame.at(index) == m_curr_frame;
}

void EventsHandler::ToggleCursor()
{
    isCursorLocked = !isCursorLocked;
    m_window.lock()->SetCursorMode(isCursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void EventsHandler::ToggleCursor(bool shouldBeVisible)
{
    isCursorLocked = shouldBeVisible;
    m_window.lock()->SetCursorMode(isCursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void EventsHandler::PullEvents()
{
    m_curr_frame++;
    mouse_dx = 0.0f;
    mouse_dy = 0.0f;

    double cur_time = glfwGetTime();

    world_dt = cur_time - m_last_time;
    m_last_time = cur_time;

    glfwPollEvents();
    m_window.lock()->Tick();
}
