#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// ==========================================
// 平台依赖引入
// ==========================================
#if defined(__linux__)
    #include <linux/input.h>
    // 某些旧版头文件可能缺少部分定义，做防错处理
    #ifndef BTN_LEFT
        #define BTN_LEFT 0x110
    #endif
#elif defined(_WIN32)
    #include <windows.h>
#endif

class KeyMapper {
public:
    // 定义映射类型
    using CodeToNameMap = std::unordered_map<int, std::string>;
    using NameToCodeMap = std::unordered_map<std::string, int>;

    // =========================================================
    // 公开接口 (API)
    // =========================================================

    // 1. 获取按键名称 (输入: 物理键值 -> 输出: 标准化名称)
    static std::string GetName(int code) {
        const auto& map = GetCodeToNameMap();
        auto it = map.find(code);
        if (it != map.end()) return it->second;
        return "UNKNOWN";
    }

    // 2. 获取按键代码 (输入: 标准化名称 -> 输出: 物理键值)
    static int GetCode(const std::string& name) {
        const auto& map = GetNameToCodeMap();
        auto it = map.find(name);
        if (it != map.end()) return it->second;
        return -1;
    }

    // 3. 直接获取 Code -> Name 映射表 (引用)
    static const CodeToNameMap& GetCodeToNameMap() {
        // C++11 保证静态局部变量线程安全初始化
        static CodeToNameMap instance = InitMap();
        return instance;
    }

    // 4. 直接获取 Name -> Code 映射表 (引用)
    static const NameToCodeMap& GetNameToCodeMap() {
        static NameToCodeMap instance = []() {
            NameToCodeMap reverseMap;
            const auto& forwardMap = GetCodeToNameMap();
            for (const auto& pair : forwardMap) {
                reverseMap[pair.second] = pair.first;
            }
            return reverseMap;
        }();
        return instance;
    }

private:
    // =========================================================
    // 内部初始化逻辑
    // =========================================================
    static CodeToNameMap InitMap() {
        CodeToNameMap m;

#if defined(__linux__)
        // ---------------- [ Linux 定义 ] ----------------

        // >>>> 鼠标 <<<<
        m[BTN_LEFT]     = "MOUSE_LEFT";
        m[BTN_RIGHT]    = "MOUSE_RIGHT";
        m[BTN_MIDDLE]   = "MOUSE_MIDDLE";
        m[BTN_SIDE]     = "MOUSE_SIDE1";
        m[BTN_EXTRA]    = "MOUSE_SIDE2";

        // >>>> 控制键 <<<<
        m[KEY_ESC]          = "ESCAPE";
        m[KEY_ENTER]        = "ENTER";
        m[KEY_SPACE]        = "SPACE";
        m[KEY_BACKSPACE]    = "BACKSPACE";
        m[KEY_TAB]          = "TAB";
        m[KEY_CAPSLOCK]     = "CAPSLOCK";

        // >>>> 修饰键 (区分左右) <<<<
        m[KEY_LEFTSHIFT]    = "L_SHIFT";
        m[KEY_RIGHTSHIFT]   = "R_SHIFT";
        m[KEY_LEFTCTRL]     = "L_CTRL";
        m[KEY_RIGHTCTRL]    = "R_CTRL";
        m[KEY_LEFTALT]      = "L_ALT";
        m[KEY_RIGHTALT]     = "R_ALT";

        // >>>> 导航与编辑 <<<<
        m[KEY_UP]           = "UP";
        m[KEY_DOWN]         = "DOWN";
        m[KEY_LEFT]         = "LEFT";
        m[KEY_RIGHT]        = "RIGHT";
        m[KEY_INSERT]       = "INSERT";
        m[KEY_DELETE]       = "DELETE";
        m[KEY_HOME]         = "HOME";
        m[KEY_END]          = "END";
        m[KEY_PAGEUP]       = "PAGE_UP";
        m[KEY_PAGEDOWN]     = "PAGE_DOWN";

        // >>>> 功能键 F1-F12 <<<<
        m[KEY_F1] = "F1"; m[KEY_F2] = "F2"; m[KEY_F3] = "F3"; m[KEY_F4] = "F4";
        m[KEY_F5] = "F5"; m[KEY_F6] = "F6"; m[KEY_F7] = "F7"; m[KEY_F8] = "F8";
        m[KEY_F9] = "F9"; m[KEY_F10] = "F10"; m[KEY_F11] = "F11"; m[KEY_F12] = "F12";

        // >>>> 数字 0-9 <<<<
        m[KEY_0] = "0"; m[KEY_1] = "1"; m[KEY_2] = "2"; m[KEY_3] = "3"; m[KEY_4] = "4";
        m[KEY_5] = "5"; m[KEY_6] = "6"; m[KEY_7] = "7"; m[KEY_8] = "8"; m[KEY_9] = "9";

        // >>>> 字母 A-Z <<<<
        m[KEY_A] = "A"; m[KEY_B] = "B"; m[KEY_C] = "C"; m[KEY_D] = "D"; m[KEY_E] = "E";
        m[KEY_F] = "F"; m[KEY_G] = "G"; m[KEY_H] = "H"; m[KEY_I] = "I"; m[KEY_J] = "J";
        m[KEY_K] = "K"; m[KEY_L] = "L"; m[KEY_M] = "M"; m[KEY_N] = "N"; m[KEY_O] = "O";
        m[KEY_P] = "P"; m[KEY_Q] = "Q"; m[KEY_R] = "R"; m[KEY_S] = "S"; m[KEY_T] = "T";
        m[KEY_U] = "U"; m[KEY_V] = "V"; m[KEY_W] = "W"; m[KEY_X] = "X"; m[KEY_Y] = "Y";
        m[KEY_Z] = "Z";

#elif defined(_WIN32)
        // ---------------- [ Windows 定义 ] ----------------
        // 必须确保这里的字符串与上方 Linux 部分完全一致！

        // >>>> 鼠标 <<<<
        m[VK_LBUTTON]   = "MOUSE_LEFT";
        m[VK_RBUTTON]   = "MOUSE_RIGHT";
        m[VK_MBUTTON]   = "MOUSE_MIDDLE";
        m[VK_XBUTTON1]  = "MOUSE_SIDE1";
        m[VK_XBUTTON2]  = "MOUSE_SIDE2";

        // >>>> 控制键 <<<<
        m[VK_ESCAPE]    = "ESCAPE";
        m[VK_RETURN]    = "ENTER";
        m[VK_SPACE]     = "SPACE";
        m[VK_BACK]      = "BACKSPACE";
        m[VK_TAB]       = "TAB";
        m[VK_CAPITAL]   = "CAPSLOCK";

        // >>>> 修饰键 (区分左右) <<<<
        m[VK_LSHIFT]    = "L_SHIFT";
        m[VK_RSHIFT]    = "R_SHIFT";
        m[VK_LCONTROL]  = "L_CTRL";
        m[VK_RCONTROL]  = "R_CTRL";
        m[VK_LMENU]     = "L_ALT";
        m[VK_RMENU]     = "R_ALT";

        // >>>> 导航与编辑 <<<<
        m[VK_UP]        = "UP";
        m[VK_DOWN]      = "DOWN";
        m[VK_LEFT]      = "LEFT";
        m[VK_RIGHT]     = "RIGHT";
        m[VK_INSERT]    = "INSERT";
        m[VK_DELETE]    = "DELETE";
        m[VK_HOME]      = "HOME";
        m[VK_END]       = "END";
        m[VK_PRIOR]     = "PAGE_UP";   // Win32 API 叫 PRIOR
        m[VK_NEXT]      = "PAGE_DOWN"; // Win32 API 叫 NEXT

        // >>>> 功能键 F1-F12 <<<<
        m[VK_F1] = "F1"; m[VK_F2] = "F2"; m[VK_F3] = "F3"; m[VK_F4] = "F4";
        m[VK_F5] = "F5"; m[VK_F6] = "F6"; m[VK_F7] = "F7"; m[VK_F8] = "F8";
        m[VK_F9] = "F9"; m[VK_F10] = "F10"; m[VK_F11] = "F11"; m[VK_F12] = "F12";

        // >>>> 字母 A-Z & 数字 0-9 <<<<
        // Windows VK 码对于字母和数字直接等于 ASCII 码
        for (char c = '0'; c <= '9'; ++c) m[c] = std::string(1, c);
        for (char c = 'A'; c <= 'Z'; ++c) m[c] = std::string(1, c);

#endif
        return m;
    }
};