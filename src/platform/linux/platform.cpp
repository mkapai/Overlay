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
        printf("Time: %.6f, Code: %d, Value: %d\n", time, code, val); 
        //todo::把当前按键存储到map中 input_key_state 读取  
        InputDevice::key_state_map[code] = InputDevice::_KEY_STATE{code,val,time};
    });
}

std::int32_t input_key_state(std::int32_t key_code)
{
    static double t = 0;
    auto it = InputDevice::key_state_map.find(key_code);
    if(it != InputDevice::key_state_map.end()){
        //300ms 内有效 t是上次触发时间 间隔要求为300ms之间
        if(t == 0){
            t = it->second.time;
            return it->second.value;
        }
        else if(it->second.time - t > 0.3){
            t = it->second.time;
            return it->second.value;
        }
    
        
    }
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
    GLFWwindow *w = (GLFWwindow *)hwnd;
    printf("Set Window Passthrough: %d\n", enable);
    if (enable)
    {
        //鼠标穿透要在设置后 鼠标转移到非当前窗口区域 或者 切换窗口才生效 寻找解决方案中...
        glfwSetWindowAttrib(w, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
        //glfwPollEvents();
        //glfwSetWindowTitle(w, "56456 un_focus"" - PiP");
        //glfwPollEvents();

    }
    else
    {
        glfwSetWindowAttrib(w, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
        //glfwSetWindowTitle(w, "56456"" - PiP");
    }
    return true;
}