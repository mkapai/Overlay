#include "platform.h"
#include <GLFW/glfw3.h>
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
    if (enable)
    {
        //不切换窗口 无法切换鼠标穿透 状态!
        glfwSetWindowAttrib((GLFWwindow *)hwnd, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
        int w, h;
        glfwGetFramebufferSize((GLFWwindow *)hwnd, &w, &h);
        glfwSetWindowSize((GLFWwindow *)hwnd, w-5, h-5);
        glfwSetWindowSize((GLFWwindow *)hwnd, w, h);
    }
    else
    {
        glfwSetWindowAttrib((GLFWwindow *)hwnd, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
    }
    return true;
}