add_rules("mode.debug", "mode.release")

package("glfw_")
    set_sourcedir(path.join(os.scriptdir(), "glfw"))
    add_configs("glfw_include", {description = "Choose submodules enabled in glfw", default = "none", type = "string", values = {"none", "vulkan", "glu", "glext", "es2", "es3", "system"}})
    add_configs("x11", {description = "Build support for X11", default = is_plat("linux"), type = "boolean"})
    add_configs("wayland", {description = "Build support for Wayland", default = false, type = "boolean"})

    if is_plat("linux") then
        add_extsources("apt::libglfw3-dev", "pacman::glfw-x11")
    end

    add_deps("cmake")
    add_deps("opengl", {optional = true})
    if is_plat("macosx") then
        add_frameworks("Cocoa", "IOKit")
    elseif is_plat("windows") then
        add_syslinks("user32", "shell32", "gdi32")
    elseif is_plat("mingw", "msys") then
        add_syslinks("gdi32")
    elseif is_plat("linux") then
        add_syslinks("dl", "pthread")
    end

    on_load(function (package)
        local glfw_include = package:config("glfw_include")
        if glfw_include ~= "system" then
            package:add("defines", "GLFW_INCLUDE_" .. glfw_include:upper())
        end
        if package:config("x11") then
            package:add("deps", "libx11", "libxrandr", "libxrender", "libxinerama", "libxfixes", "libxcursor", "libxi", "libxext")
        end
        if package:config("wayland") then
            package:add("deps", "wayland", "wayland-protocols")
        end

        if package:is_plat("windows") and package:config("shared") then
            package:add("defines", "GLFW_DLL")
        end
    end)

    on_install("!wasm and !iphoneos", function (package)
        local configs = {"-DGLFW_BUILD_DOCS=OFF", "-DGLFW_BUILD_TESTS=OFF", "-DGLFW_BUILD_EXAMPLES=OFF"}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        if package:is_plat("windows") then
            table.insert(configs, "-DUSE_MSVC_RUNTIME_LIBRARY_DLL=" .. (package:has_runtime("MT") and "OFF" or "ON"))
        end
        table.insert(configs, "-DGLFW_BUILD_X11=" .. (package:config("x11") and "ON" or "OFF"))
        table.insert(configs, "-DGLFW_BUILD_WAYLAND=" .. (package:config("wayland") and "ON" or "OFF"))
        if package:is_plat("linux") then
            import("package.tools.cmake").install(package, configs, {packagedeps = {"libxrender", "libxfixes", "libxext", "libx11", "wayland"}})
        else
            import("package.tools.cmake").install(package, configs)
        end
    end)

    on_test(function (package)
        assert(package:check_csnippets({test = [[
            void test() {
                glfwInit();
                glfwTerminate();
            }
        ]]}, {configs = {languages = "c11"}, includes = "GLFW/glfw3.h"}))
    end)
package_end()

add_requires("vulkansdk")
if is_plat("linux") then
    add_requires("libxkbcommon")
    add_requires("glfw_",{configs={wayland=true}})
else
    add_requires("glfw_")
end
target("Overlay")
    set_languages("c99","c++20")
    set_kind("binary")


    if is_plat("linux") then
        add_packages('libxkbcommon')
        add_defines("__linux__")
        add_includedirs("src/platform/linux/")
        add_files("src/platform/linux/**.cpp")

    else 
        add_defines("__windows__")
        add_includedirs("src/platform/windows/")
        add_files("src/platform/windows/**.cpp")
    end 
    add_includedirs("src/imgui/","src/imgui/backends/","src/platform/")
    add_files("src/main.cpp","src/imgui/**.cpp")
    add_packages('glfw_','vulkansdk')

    add_defines('APP_USE_UNLIMITED_FRAME_RATE')
    if is_mode("release") then
        set_optimize("fastest")
        add_vectorexts("avx2")
    end
    if is_mode("debug") then 
        add_defines("_DEBUG")
        set_symbols("debug")
    end



--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

