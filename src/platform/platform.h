#pragma once
#include <chrono>
#define NowTime std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now().time_since_epoch()).count()


#include <string>
#include <cstdint>
#include "input_code.h"
//跨平台头文件 声明非平台相关的接口 由各平台实现CPP文件实现




//初始化平台相关输入系统
bool input_init();

//获取按键状态 3个状态 0未按下 1按下 2持续按下
std::int32_t input_key_state(std::int32_t key_code);
//获取按键状态
std::int32_t input_name_state(std::string key_name);

//获取按键名称
std::string input_key_name(std::int32_t key_code);

std::int32_t input_name_key(std::string key_name);




//让窗口穿透!
bool windows_passthrough(std::uintptr_t hwnd, bool enable);