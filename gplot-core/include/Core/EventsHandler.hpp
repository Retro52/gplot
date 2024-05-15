#include <array>
#include <memory>

#include <GLFW/glfw3.h>
#include <Core/Window.hpp>

namespace gplot::core
{
    constexpr int MOUSE_BUTTONS_OFFSET = 1024;

    class EventsHandler
    {
    public:
        inline static float mouse_x;
        inline static float mouse_y;
        inline static float mouse_dx;
        inline static float mouse_dy;
        inline static double world_dt;
        inline static bool isCursorLocked;
        inline static bool isCursorStarted;

        /* Restriction to create an instance of this class */
        EventsHandler() = delete;
        EventsHandler(EventsHandler&&) = delete;
        EventsHandler(const EventsHandler&) = delete;

        /**
         * Initializes event handler and sets callbacks for actions
         * @return exit code
         */
        static int Initialize(const std::shared_ptr<gplot::core::Window>& window);

        /**
         * UpdateModelMatrix all events, called once every frame
         */
        static void PullEvents();

        /**
         * Check if key is pressed
         * @param keycode key id
         * @return true if pressed, false otherwise
         */
        static bool IsPressed(int keycode);

        /**
         * Chek if key was just pressed
         * @param keycode key id
         * @return true if was just pressed, false otherwise
         */
        static bool IsJustPressed(int keycode);

        /**
         * Check if mouse button is clicked
         * @param button button id
         * @return true if is clicked, false otherwise
         */
        static bool IsClicked(int button);

        /**
         * Check if mouse button was just clicked
         * @param button button id
         * @return true if was just clicked, false otherwise
         */
        [[maybe_unused]] static bool IsJustClicked(int button);

        /**
         * Switch hide & show cursor modes
         */
        static void ToggleCursor();

        static void ToggleCursor(bool shouldBeVisible);

        static void MouseCallback(GLFWwindow * window, int button, int action, int mode);

        static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);

        static void CursorCallback(GLFWwindow * window, double xpos, double ypos);

        static void WindowCallback(GLFWwindow * /*window*/, int width, int height);
    private:

        inline static std::weak_ptr<gplot::core::Window> m_window;

        inline static double m_last_time;
        inline static unsigned int m_curr_frame;
        inline static std::array<bool, 1032> m_keys_state;
        inline static std::array<unsigned int, 1032> m_keys_state_frame;
    };


    enum Key : unsigned int
    {
        A             = GLFW_KEY_A,
        B             = GLFW_KEY_B,
        C             = GLFW_KEY_C,
        D             = GLFW_KEY_D,
        E             = GLFW_KEY_E,
        F             = GLFW_KEY_F,
        G             = GLFW_KEY_G,
        H             = GLFW_KEY_H,
        I             = GLFW_KEY_I,
        J             = GLFW_KEY_J,
        K             = GLFW_KEY_K,
        L             = GLFW_KEY_L,
        M             = GLFW_KEY_M,
        N             = GLFW_KEY_N,
        O             = GLFW_KEY_O,
        P             = GLFW_KEY_P,
        Q             = GLFW_KEY_Q,
        R             = GLFW_KEY_R,
        S             = GLFW_KEY_S,
        T             = GLFW_KEY_T,
        U             = GLFW_KEY_U,
        V             = GLFW_KEY_V,
        W             = GLFW_KEY_W,
        X             = GLFW_KEY_X,
        Y             = GLFW_KEY_Y,
        Z             = GLFW_KEY_Z,

        /* Keypad keys */
        KP0           = GLFW_KEY_KP_0,
        KP1           = GLFW_KEY_KP_1,
        KP2           = GLFW_KEY_KP_2,
        KP3           = GLFW_KEY_KP_3,
        KP4           = GLFW_KEY_KP_4,
        KP5           = GLFW_KEY_KP_5,
        KP6           = GLFW_KEY_KP_6,
        KP7           = GLFW_KEY_KP_7,
        KP8           = GLFW_KEY_KP_8,
        KP9           = GLFW_KEY_KP_9,
        KPAdd         = GLFW_KEY_KP_ADD,
        KPEnter       = GLFW_KEY_KP_ENTER,
        KPDivide      = GLFW_KEY_KP_DIVIDE,
        KPMultiply    = GLFW_KEY_KP_MULTIPLY,
        KPSubtract    = GLFW_KEY_KP_SUBTRACT,

        /* Numbers panel */
        NUM0          = GLFW_KEY_0,
        NUM1          = GLFW_KEY_1,
        NUM2          = GLFW_KEY_2,
        NUM3          = GLFW_KEY_3,
        NUM4          = GLFW_KEY_4,
        NUM5          = GLFW_KEY_5,
        NUM6          = GLFW_KEY_6,
        NUM7          = GLFW_KEY_7,
        NUM8          = GLFW_KEY_8,
        NUM9          = GLFW_KEY_9,
        F1            = GLFW_KEY_F1,
        F2            = GLFW_KEY_F2,
        F3            = GLFW_KEY_F3,
        F4            = GLFW_KEY_F4,
        F5            = GLFW_KEY_F5,
        F6            = GLFW_KEY_F6,
        F7            = GLFW_KEY_F7,
        F8            = GLFW_KEY_F8,
        F9            = GLFW_KEY_F9,
        F10           = GLFW_KEY_F10,
        F11           = GLFW_KEY_F11,
        F12           = GLFW_KEY_F12,
        F13           = GLFW_KEY_F13,
        F14           = GLFW_KEY_F14,
        F15           = GLFW_KEY_F15,
        F16           = GLFW_KEY_F16,
        F17           = GLFW_KEY_F17,
        F18           = GLFW_KEY_F18,
        F19           = GLFW_KEY_F19,
        F20           = GLFW_KEY_F20,
        F21           = GLFW_KEY_F21,
        F22           = GLFW_KEY_F22,
        F23           = GLFW_KEY_F23,
        F24           = GLFW_KEY_F24,
        F25           = GLFW_KEY_F25,

        Up            = GLFW_KEY_UP,
        Down          = GLFW_KEY_DOWN,
        Left          = GLFW_KEY_LEFT,
        Right         = GLFW_KEY_RIGHT,

        Tab           = GLFW_KEY_TAB,
        Enter         = GLFW_KEY_ENTER,
        Space         = GLFW_KEY_SPACE,
        Delete        = GLFW_KEY_DELETE,
        Backspace     = GLFW_KEY_BACKSPACE,

        LeftAlt       = GLFW_KEY_LEFT_ALT,
        RightAlt      = GLFW_KEY_RIGHT_ALT,
        LeftShift     = GLFW_KEY_LEFT_SHIFT,
        RightShift    = GLFW_KEY_RIGHT_SHIFT,
        LeftControl   = GLFW_KEY_LEFT_CONTROL,
        RightControl  = GLFW_KEY_RIGHT_CONTROL,
    };

    enum Mouse : unsigned int
    {
        Mouse1 = GLFW_MOUSE_BUTTON_1,
        Mouse2 = GLFW_MOUSE_BUTTON_2,
        Mouse3 = GLFW_MOUSE_BUTTON_3,
        Mouse4 = GLFW_MOUSE_BUTTON_4,
        Mouse5 = GLFW_MOUSE_BUTTON_5,
        Mouse6 = GLFW_MOUSE_BUTTON_6,
        Mouse7 = GLFW_MOUSE_BUTTON_7,
        Mouse8 = GLFW_MOUSE_BUTTON_8,
    };
}
