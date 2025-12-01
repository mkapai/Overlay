#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <cstring>
#include <sys/epoll.h>
#include <thread>
#include <functional>

namespace fs = std::filesystem;

// 辅助宏：用于检查位掩码中的某一位是否被置位
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x) - 1) / BITS_PER_LONG) + 1)
#define OFF(x) ((x) % BITS_PER_LONG)
#define BIT(x) (1UL << OFF(x))
#define LONG(x) ((x) / BITS_PER_LONG)
#define test_bit(bit, array) ((array[LONG(bit)] >> OFF(bit)) & 1)

class InputDevice
{
public:
    struct _KEY_STATE
    {
        int code;
        int value;
        double time;
    }__KEY_STATE;
    static inline std::unordered_map<int, struct _KEY_STATE> key_state_map{};

    /// @brief 启动输入设备监听线程 传入一个回调函数 用于处理按键事件
    /// @param cb: 回调函数，签名为 void(double time, int code, int val)
    /// @return
    static bool start(std::function<void(double, int, int)> cb)
    {
        event_callback_ = std::move(cb);
        std::string base_path = "/dev/input/";
        for (const auto &entry : fs::directory_iterator(base_path))
        {
            std::string path = entry.path().string();
            if (path.find("event") == std::string::npos)
                continue;
            int fd = open(path.c_str(), O_RDONLY);
            if (fd < 0)
            {
                continue;
            }
            char name[256] = "Unknown";
            ioctl(fd, EVIOCGNAME(sizeof(name)), name);
            bool is_keyboard = check_is_keyboard(fd);
            if (is_keyboard && keyboard_fd == -1)
            {

                printf("is_keyboard Found input device: %s (%s)\n", path.c_str(), name);
                keyboard_fd = fd;
                continue;
            }
            bool is_mouse = check_is_mouse(fd);
            if (is_mouse && mouse_fd == -1)
            {

                printf("is_mouse Found input device: %s (%s)\n", path.c_str(), name);
                mouse_fd = fd;
                continue;
            }
            if (mouse_fd != -1 && keyboard_fd != -1)
                break;
            close(fd);
        }

        if (mouse_fd == -1)
        {
            perror("No mouse device found!\n");
            abort();
        }
        if (keyboard_fd == -1)
        {
            perror("No keyboard device found!\n");
            abort();
        }

        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ == -1)
        {
            perror("epoll_create1");
            abort();
        }

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = keyboard_fd;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, keyboard_fd, &ev) == -1)
        {
            perror("keyboard_fd epoll_ctl: add");
            abort();
        }

        struct epoll_event ev1;
        ev1.events = EPOLLIN;
        ev1.data.fd = mouse_fd;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, mouse_fd, &ev1) == -1)
        {
            perror("mouse_fd epoll_ctl: add");
            abort();
        }

        running_ = true;
        worker_thread_ = std::thread([&]
                                     {
            while (running_){
                struct epoll_event events[2];
                int nfds = epoll_wait(epoll_fd_, events, 2, 1000);
                if (nfds < 0)
                    continue;
                for (int n = 0; n < nfds; ++n)
                {
                    int fd = events[n].data.fd;
                    if (fd == keyboard_fd)
                    {
                        struct input_event iev;
                        ssize_t bytes = read(fd, &iev, sizeof(iev));
                        if (bytes == sizeof(iev))
                        {
                           if (iev.type == EV_KEY)
                           {
                               double t = iev.time.tv_sec + iev.time.tv_usec / 1e6;
                               if (event_callback_)
                                   event_callback_(t, iev.code, iev.value);
                           }
                        }
                    }
                    else if (fd == mouse_fd)
                    {
                        struct input_event iev;
                        ssize_t bytes = read(fd, &iev, sizeof(iev));
                        if (bytes == sizeof(iev))
                        {
                            // 处理鼠标事件 ev
                            if (iev.type == EV_KEY)
                            {
                                double t = iev.time.tv_sec + iev.time.tv_usec / 1e6;
                                if (event_callback_)
                                    event_callback_(t, iev.code, iev.value);
                            }
                        }
                    }
                }

            } });
        return true;
    }

    static void stop()
    {
        running_ = false;
        if (worker_thread_.joinable())
            worker_thread_.join();
        if (mouse_fd != -1)
        {
            close(mouse_fd);
            mouse_fd = -1;
        }
        if (keyboard_fd != -1)
        {
            close(keyboard_fd);
            keyboard_fd = -1;
        }
        if (epoll_fd_ != -1)
        {
            close(epoll_fd_);
            epoll_fd_ = -1;
        }
    }

private:
    static inline int epoll_fd_ = -1;
    static inline bool running_ = false;
    static inline std::thread worker_thread_;
    static inline int mouse_fd = -1;
    static inline int keyboard_fd = -1;
    static inline std::function<void(double, int, int)> event_callback_ = nullptr;
    static bool check_is_mouse(int fd)
    {
        unsigned long evbit[NBITS(EV_MAX)] = {0};
        if (ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), evbit) < 0)
            return false;

        // 1. 必须支持 EV_REL (相对位移) 和 EV_KEY (按键)
        if (test_bit(EV_REL, evbit) && test_bit(EV_KEY, evbit))
        {

            // 2. 检查 X轴 和 Y轴
            unsigned long relbit[NBITS(REL_MAX)] = {0};
            ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relbit)), relbit);

            // 3. 检查 鼠标左键 (BTN_LEFT)
            unsigned long keybit[NBITS(KEY_MAX)] = {0};
            ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit);

            if (test_bit(REL_X, relbit) && test_bit(REL_Y, relbit) && test_bit(BTN_LEFT, keybit))
            {
                return true;
            }
        }
        return false;
    }
    static bool check_is_keyboard(int fd)
    {
        unsigned long evbit[NBITS(EV_MAX)] = {0};
        if (ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), evbit) < 0)
            return false;
        // 检查是否支持相对移动 支持就肯定不是键盘
        if (test_bit(EV_REL, evbit))
            return false;

        bool has_keys = false;
        bool has_leds = false;

        // --- 检查按键 ---
        if (test_bit(EV_KEY, evbit))
        {
            unsigned long keybit[NBITS(KEY_MAX)] = {0};
            ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit);
            // 打印下看看到底有什么键

            // for (int i = 0; i < KEY_MAX; i++) {
            //     if (test_bit(i, keybit)) {
            //         printf("Key code %d is present\n", i);
            //     }
            // }

            if (test_bit(KEY_ESC, keybit) &&
                test_bit(KEY_1, keybit) &&
                test_bit(KEY_A, keybit) &&
                test_bit(KEY_ENTER, keybit) &&
                test_bit(KEY_CAPSLOCK, keybit))
            {
                has_keys = true;
            }
        }

        // --- 检查 LED (CAPSLOCK 灯) --- 万一没有咋办??
        if (test_bit(EV_LED, evbit))
        {
            unsigned long ledbit[NBITS(LED_MAX)] = {0};
            ioctl(fd, EVIOCGBIT(EV_LED, sizeof(ledbit)), ledbit);

            if (test_bit(LED_CAPSL, ledbit))
            { // 键盘灯检测
                has_leds = true;
            }
        }
        // 判定逻辑：
        // 如果有常见的字母/数字键，我们认为它是键盘。
        // 如果它有 CapsLock 灯，它极大概率也是键盘。
        // 注意：某些迷你键盘可能没有 LED，所以 has_leds 不是必须的，但它是加分项。
        return has_keys && has_leds;
    }
};