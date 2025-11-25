# Overlay

项目简介
---
这是一个使用 ImGui + Vulkan 的示例/演示程序（项目源码位于 `src/`，ImGui 集成在 `imgui/`）。项目使用 `xmake` 作为构建系统，目标可在 Linux、Windows、macOS 上编译（需要 Vulkan 支持或对应的 MoltenVK）。

主要目录
---
- `src/`：程序源代码（入口 `main.cpp`）。
- `imgui/`：集成的 Dear ImGui 源代码与 Vulkan/GLFW 后端。
- `imgui/backends/vulkan/`：Vulkan 相关 shader 脚本与示例 shader。

构建概览
---
项目使用 `xmake` 构建。构建一般流程：

```bash
# 配置（可选：指定平台、架构与构建类型）
xmake f -p linux -a x86_64 -m debug
# 编译
xmake
# 可执行文件位于 build/<platform>/<arch>/<mode>/Overlay
```

示例运行（Linux 默认输出路径示例）：

```bash
./build/linux/x86_64/debug/Overlay
```

各平台依赖与安装
---
以下为常见平台的依赖说明与常用安装命令示例。请根据你使用的发行版或包管理器做相应调整，或直接参考官方文档（Vulkan SDK、GLFW、xmake）。

Linux（Debian / Ubuntu 为例）
- **必备**: `xmake`, `libxkbcommon`,`Vulkan SDK`
- **示例命令**：

```bash
# 安装 xmake（官方安装脚本）
curl -fsSL https://xmake.io/shget.text | bash
# 更新包索引并安装依赖 xmake 能处理一部分包 但是libxkbcommon包xamke无法处理 我们就本机安装
sudo apt update
sudo apt install -y libvulkan1 libvulkan-dev vulkan-tools vulkan-validationlayers mesa-vulkan-drivers libxkbcommon-dev 
```

Windows
- Xmake直接编译,确保你拥有任意编译链

构建与调试建议
---
- 若使用 Vulkan，请确保显卡驱动支持 Vulkan 并正确安装 Vulkan loader（Linux 上可用 `vulkaninfo` 检查）。
- 在开发阶段启用 validation layers（Vulkan SDK 自带）以捕捉错误。Linux/Windows 上通常通过设置 `VK_LAYER_PATH` 或安装 SDK 自动配置。
- 如果遇到 shader 编译问题，确认 `glslangValidator` 在 `PATH` 中或使用 Vulkan SDK 提供的编译器（`glslc`/`glslangValidator`）。

常见问题与排查
---
- **程序运行但窗口无法显示/崩溃**：检查 Vulkan 驱动、验证层输出，运行 `vulkaninfo`（或 Windows 的 Vulkan SDK 工具）查看设备与实例支持情况。
- **找不到 GLFW / 链接错误**：确认已安装对应开发包（`libglfw3-dev` / vcpkg 或手动构建并将路径暴露给 `xmake`）。
- **Linux上背景不透明**：需要确认目标Surface是否支持*VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR*或*VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR*,目前使用*VK_KHR_wayland_surface*,可能在某些驱动或者平台上不支持!
- **Linux窗口无法置顶**: Gnome 可以安装插件pip-on-top,其他需要自己设置!(wayland不允许窗口自己设置!)


ImGui 源码修改说明
---
- 仓库包含 Dear ImGui 的源码在 `imgui/` 目录下。为便于本项目集成，仓库只对 ImGui 的 Vulkan 后端实现做了局部修改。
- 仅修改的文件：`imgui/backends/imgui_impl_vulkan.cpp`。
- 这些修改用于项目的 Vulkan 后端集成（构建/兼容性/行为调整）。

许可证
---
请检查项目中各第三方库（ImGui、GLFW、Vulkan SDK 等）的许可约束。本 README 并不改变各组件的许可证。
