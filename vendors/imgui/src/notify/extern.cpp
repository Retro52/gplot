#include <imgui.h>
#include <notify/imgui_notify.h>

#if __cplusplus < 201703L
namespace ImGui
{
    std::vector<ImGuiToast> notifications;
}
#endif
