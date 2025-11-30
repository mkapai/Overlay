#pragma once
#include <cstdint>
//跨平台头文件 声明非平台相关的接口 由各平台实现CPP文件实现

//初始化平台相关输入系统
bool input_init();

//获取按键状态 3个状态 0未按下 1按下 2持续按下
std::int32_t input_key_state(std::int32_t key_code);


//让窗口穿透!
bool windows_passthrough(std::uintptr_t hwnd, bool enable);