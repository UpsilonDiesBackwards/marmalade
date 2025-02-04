cmake_minimum_required(VERSION 3.28)
project(marmalade)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

set(LUA_VERSION "lua-5.4.7")
# Lua libraries, if on Windows
set(LUA_SRC_DIR "../lua/src/lua")
set(LUA_LIB "../lua/lib/Lua.lib")

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/json)
add_subdirectory(vendor/glm)

set(USE_NSEC OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/libgit2)

find_package(OpenGL REQUIRED)

if (NOT WIN32)
    # If we're not on Windows, lua can be compiled automatically
    ExternalProject_Add(lua
            PREFIX ${CMAKE_BINARY_DIR}/${LUA_VERSION}
            URL "https://www.lua.org/ftp/${LUA_VERSION}.tar.gz"
            CONFIGURE_COMMAND ""
            BUILD_COMMAND make generic
            BUILD_ALWAYS true
            BUILD_IN_SOURCE true
            INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(lua BINARY_DIR)
    set(LUA_INCLUDE_DIR ${BINARY_DIR}/src)
    set(LUA_LIBRARIES ${BINARY_DIR}/src/liblua.a)
else ()
    set(LUA_INCLUDE_DIR ${LUA_SRC_DIR})
    set(LUA_LIBRARIES ${LUA_LIB})
endif ()

file(GLOB IMGUI_SRC "vendor/imgui/*.h" "vendor/imgui/*.cpp")
list(APPEND IMGUI_SRC vendor/imgui/backends/imgui_impl_glfw.h vendor/imgui/backends/imgui_impl_glfw.cpp vendor/imgui/backends/imgui_impl_opengl3.h vendor/imgui/backends/imgui_impl_opengl3.cpp)

list(APPEND ENGINE_API include/archetypes/camera.h include/archetypes/entity.h
        include/graphics/renderable.h include/graphics/shader.h include/graphics/texture.h
        include/graphics/framebuffer.h include/graphics/multisampledframebuffer.h
        include/io/input.h include/io/inputmanager.h
        include/scene/scene.h include/scene/scenemanager.h
)

list(APPEND ENGINE_SRC src/gui/imgui/editor.cpp
        src/gui/imgui/editor.h
        src/gui/imgui/editorviews.cpp
        src/gui/imgui/editorviews.h
        src/gui/imgui/editorviewstatusbar.cpp
        src/gui/imgui/editorviewstatusbar.h
        src/gui/imgui/scenehierarchy.cpp
        src/gui/imgui/scenehierarchy.h
        src/gui/imgui/topbar.cpp
        src/gui/imgui/topbar.h
        src/gui/editorviews.h
        src/gui/editview.cpp
        src/gui/editview.h
        src/graphics/multisampledframebuffer.cpp
        src/gui/gameview.cpp
        src/gui/gameview.h
        src/gui/imgui
        src/gui/stylemanager.cpp
        src/gui/stylemanager.h
        src/gui/imgui/packagemanager.cpp
        src/gui/imgui/packagemanager.h
        src/application/guilogsink.cpp
        src/application/guilogsink.h
        src/gui/imgui/log.cpp
        src/gui/imgui/log.h
        src/gui/window.h
        src/gui/imgui/projectwizard.cpp
        src/gui/imgui/projectwizard.h
)

list(APPEND ENGINE_SRC src/application/application.cpp src/application/application.h src/application/profiler.cpp src/application/profiler.h
        src/archetypes/camera.cpp src/archetypes/entity.cpp
        src/graphics/renderable.cpp src/graphics/shader.cpp src/graphics/texture.cpp src/graphics/framebuffer.cpp
        src/io/input.cpp src/io/inputmanager.cpp
        src/scene/scene.cpp src/scene/scenemanager.cpp
        src/application/project.cpp
        src/application/project.h
)

add_executable(marmalade vendor/glad/src/glad.c ${IMGUI_SRC} vendor/imguifiledialog/ImGuiFileDialog.cpp
        ${ENGINE_API}
        ${ENGINE_SRC}
        src/main.cpp
)

if (NOT WIN32)
    add_dependencies(marmalade lua)
endif ()

file(COPY vendor/imgui/misc/fonts/ProggyClean.ttf DESTINATION ${CMAKE_BINARY_DIR}/resources/fonts)
file(COPY vendor/imgui/misc/fonts/Roboto-Medium.ttf DESTINATION ${CMAKE_BINARY_DIR}/resources/fonts)
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(marmalade PRIVATE glfw OpenGL::GL ${OPENGL_gl_LIBRARY} nlohmann_json::nlohmann_json glm::glm ${LUA_LIBRARIES} libgit2package)

if (WIN32)
    target_link_libraries(marmalade PRIVATE Shcore.lib ws2_32.lib)
endif ()

target_include_directories(marmalade PRIVATE vendor/glad/include vendor/glfw-3.4/include vendor/imgui vendor/imguifiledialog glm ${LUA_INCLUDE_DIR} vendor/luaaa vendor/stb vendor/libgit2/include vendor/iconfontcppheaders vendor/spdlog/include)
target_link_directories(marmalade PRIVATE vendor/nativefiledialog/lib vendor/luaaa)
