#include "platform.h"
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "input_device.h"

bool input_init()
{
    return InputDevice::start([](double time, int code, int val)
                              {
        //处理按键事件
        printf("Time: %.6f, Code: %d, Value: %d\n", time, code, val); });
}

std::int32_t input_key_state(std::int32_t key_code)
{

    return 0;
}
std::int32_t input_name_state(std::string key_name)
{
    int code = input_name_key(key_name);
    return input_key_state(code);
}

std::string input_key_name(std::int32_t key_code)
{
    return KeyMapper::GetName(key_code);
}

std::int32_t input_name_key(std::string key_name)
{
    return KeyMapper::GetCode(key_name);
}

bool windows_passthrough(std::uintptr_t hwnd, bool enable)
{
    GLFWwindow* w = (GLFWwindow *)hwnd;
    if (enable)
    {
        //不切换窗口 无法切换鼠标穿透 状态!
        glfwSetWindowAttrib(w, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
        auto surface = glfwGetWaylandWindow(w); //触发commit
        //wl_surface_commit(surface);

    }
    else
    {
        glfwSetWindowAttrib(w, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
        auto surface = glfwGetWaylandWindow(w); //触发commit
        //wl_surface_commit(surface);
    }
    return true;
}