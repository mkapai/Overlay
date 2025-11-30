#include "platform.h"
#include <windows.h>

bool input_init()
{
    //Windows平台暂不需要初始化
    return true;
}

std::int32_t input_key_state(std::int32_t key_code)
{
    //使用GetAsyncKeyState获取按键状态
    auto state = GetAsyncKeyState(key_code);
    if (state & 0x8000) //最高位表示按键是否被按下
    {
        if (state & 0x0001) //最低位表示按键是否在上次调用后被切换状态
            return 1; //按下
        else
            return 2; //持续按下
    }
    return 0; //未按下
}

bool windows_passthrough(std::uintptr_t hwnd, bool enable)
{
    //设置窗口扩展样式 让窗口穿透鼠标事件
    HWND hWnd = reinterpret_cast<HWND>(hwnd);
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    if (enable)
    {
        exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
    }
    else
    {
        exStyle &= ~(WS_EX_TRANSPARENT | WS_EX_LAYERED);
    }
    SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);
    return true;
}