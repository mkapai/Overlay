#include "platform.h"
#include <GLFW/glfw3.h>

bool input_init()
{
    //Linux平台需要启动线程从/dev/input/event 中获取鼠标和键盘设备  并且监听按键状态
    return true;
}


std::int32_t input_key_state(std::int32_t key_code)
{
    //待完成
    return 0;
}

bool windows_passthrough(std::uintptr_t hwnd, bool enable)
{
    if (enable)
        glfwSetWindowAttrib((GLFWwindow*)hwnd, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
    else          
        glfwSetWindowAttrib((GLFWwindow*)hwnd, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
    return true;
}