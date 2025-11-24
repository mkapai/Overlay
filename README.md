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

着色器（SPV）
---
项目中包含 Vulkan 着色器源文件，脚本 `imgui/backends/vulkan/generate_spv.sh` 用于生成 SPV 二进制。该脚本依赖 `glslangValidator` 或 Vulkan SDK 中的 shader 编译工具。

各平台依赖与安装
---
以下为常见平台的依赖说明与常用安装命令示例。请根据你使用的发行版或包管理器做相应调整，或直接参考官方文档（Vulkan SDK、GLFW、xmake）。

Linux（Debian / Ubuntu 为例）
- **必备**: `xmake`, `build-essential`/`clang`, `glfw` 开发包, `vulkan` 开发包, `glslangValidator`（或 Vulkan SDK）
- **示例命令**：

```bash
# 更新包索引并安装依赖（包含 libxkbcommon-dev）
sudo apt update
sudo apt install -y build-essential git xz-utils curl libglfw3-dev libxkbcommon-dev libvulkan-dev vulkan-utils glslang-tools
# 安装 xmake（官方安装脚本）
curl -fsSL https://xmake.io/shget.text | bash
```

其他发行版：
- Fedora: `sudo dnf install xmake glfw-devel vulkan-devel glslang`
- Arch: `sudo pacman -S xmake glfw vulkan-headers vulkan-tools glslang`

Windows
- **必备**: Visual Studio（含 Desktop 开发用 C++ 工具集）或 MSVC 工具链、Vulkan SDK（LunarG）、GLFW（可用 vcpkg 安装）、xmake。
- **建议安装方式**：
  - 安装 Visual Studio 2022（选择「使用 C++ 的桌面开发」工作负载）。
  - 从 LunarG 下载并安装 Vulkan SDK（会设置 `%VULKAN_SDK%` 环境变量）。
  - 使用 vcpkg 或手动获取 `glfw`。

```powershell
# 在 Windows 上可以通过 Scoop/Chocolatey 安装 xmake，或使用 xmake 官方安装方法
# 示例（仅示意）：
# scoop install xmake
# choco install xmake

# 使用 vcpkg 安装库（示例）
.\vcpkg\vcpkg.exe install glfw3
```

然后在 Visual Studio 的「x64 Native Tools Command Prompt」或通过 `xmake` 在带有 MSVC 环境的终端中运行 `xmake`。

macOS
- **必备**: Xcode（命令行工具）、xmake、GLFW、Vulkan 支持（通常通过 LunarG Vulkan SDK + MoltenVK）
- **示例命令（Homebrew）**：

```bash
# 安装 xmake 与 glfw
brew install xmake glfw
# Vulkan: 建议从 LunarG 官网下载 macOS 版 Vulkan SDK（包含 MoltenVK）并安装
```

构建与调试建议
---
- 若使用 Vulkan，请确保显卡驱动支持 Vulkan 并正确安装 Vulkan loader（Linux 上可用 `vulkaninfo` 检查）。
- 在开发阶段启用 validation layers（Vulkan SDK 自带）以捕捉错误。Linux/Windows 上通常通过设置 `VK_LAYER_PATH` 或安装 SDK 自动配置。
- 如果遇到 shader 编译问题，确认 `glslangValidator` 在 `PATH` 中或使用 Vulkan SDK 提供的编译器（`glslc`/`glslangValidator`）。

常见问题与排查
---
- **程序运行但窗口无法显示/崩溃**：检查 Vulkan 驱动、验证层输出，运行 `vulkaninfo`（或 Windows 的 Vulkan SDK 工具）查看设备与实例支持情况。
- **找不到 GLFW / 链接错误**：确认已安装对应开发包（`libglfw3-dev` / vcpkg 或手动构建并将路径暴露给 `xmake`）。
- **Linux上背景不透明**：需要确认目标Surface是否支持*VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR*或*VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR*,目前使用VK_KHR_wayland_surface 可能在某些驱动或者平台上不支持!


ImGui 源码修改说明
---
- 仓库包含 Dear ImGui 的源码在 `imgui/` 目录下。为便于本项目集成，仓库只对 ImGui 的 Vulkan 后端实现做了局部修改。
- 仅修改的文件：`imgui/backends/imgui_impl_vulkan.cpp`。
- 这些修改用于项目的 Vulkan 后端集成（构建/兼容性/行为调整）。

许可证
---
请检查项目中各第三方库（ImGui、GLFW、Vulkan SDK 等）的许可约束。本 README 并不改变各组件的许可证。
