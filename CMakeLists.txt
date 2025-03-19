cmake_minimum_required(VERSION 3.28)
project(marmalade)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

add_compile_definitions($<IF:$<CONFIG:Debug>,DEBUG,>)

option(USE_SYSTEM_GLFW "Use system-installed GLFW instead of bundled one" ON)
option(USE_SYSTEM_GLM "Use system-installed GLM instead of bundled one" ON)
option(USE_SYSTEM_SPDLOG "Use system-installed spdlog instead of bundled one" ON)
option(USE_SYSTEM_JSON "Use system-installed nlohmann::json instead of bundled one" ON)
option(USE_SYSTEM_LIBGIT "Use system-installed libgit2 instead of bundled one" ON)

if (USE_SYSTEM_GLFW)
    find_package(glfw3 3.3...3.5 QUIET)
    if (NOT glfw3_FOUND)
        message(STATUS "System GLFW not found. Falling back to bundled GLFW.")
        set(USE_SYSTEM_GLFW OFF)
    else ()
        message(STATUS "Using system GLFW.")
    endif ()
endif ()

if (NOT USE_SYSTEM_GLFW)
    message(STATUS "Using bundled GLFW")
    add_subdirectory(vendor/glfw)
endif ()

if (USE_SYSTEM_GLM)
    find_package(glm QUIET)
    if (NOT glm_FOUND)
        message(STATUS "System GLM not found. Falling back to bundled GLM.")
        set(USE_SYSTEM_GLM OFF)
    else ()
        message(STATUS "Using system GLM.")
    endif ()
endif ()

if (NOT USE_SYSTEM_GLM)
    message(STATUS "Using bundled GLM")
    add_subdirectory(vendor/glm)
endif ()

if (USE_SYSTEM_SPDLOG)
    find_package(spdlog QUIET)
    if (NOT spdlog_FOUND)
        message(STATUS "System spdlog not found. Falling back to bundled spdlog.")
        set(USE_SYSTEM_SPDLOG OFF)
    else ()
        message(STATUS "Using system spdlog.")
    endif ()
endif ()

if (NOT USE_SYSTEM_SPDLOG)
    message(STATUS "Using bundled spdlog")
    add_subdirectory(vendor/spdlog)
endif ()

if (USE_SYSTEM_JSON)
    find_package(nlohmann_json CONFIG QUIET)
    if (NOT nlohmann_json_FOUND)
        message(STATUS "System nlohmann::json not found. Falling back to bundled nlohmann::json.")
        set(USE_SYSTEM_JSON OFF)
    else ()
        message(STATUS "Using system nlohmann::json.")
    endif ()
endif ()

if (NOT USE_SYSTEM_JSON)
    message(STATUS "Using bundled nlohmann::json")
    add_subdirectory(vendor/json)
endif ()

# libgit2 options
set(USE_NSEC OFF)
set(USE_SSH ON)
if (WIN32)
    set(USE_SSH OFF)
endif ()
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if (USE_SYSTEM_LIBGIT)
    find_package(libgit2 QUIET)
    if (NOT libgit2_FOUND)
        message(STATUS "System libgit2 not found. Falling back to bundled libgit2.")
        set(USE_SYSTEM_LIBGIT OFF)
    else ()
        message(STATUS "Using system libgit2.")
    endif ()
endif ()

if (NOT USE_SYSTEM_LIBGIT)
    message(STATUS "Using bundled libgit2")
    add_subdirectory(vendor/libgit2)
endif ()

set(LUA_VERSION "lua-5.4.7" CACHE STRING "Lua version")
# Lua libraries, if on Windows
set(LUA_SRC_DIR "../lua/src/lua" CACHE PATH "Lua source directory")
set(LUA_LIB "../lua/lib/Lua.lib" CACHE FILEPATH "Lua library file")

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
list(APPEND IMGUI_SRC
        vendor/imgui/backends/imgui_impl_glfw.h
        vendor/imgui/backends/imgui_impl_glfw.cpp
        vendor/imgui/backends/imgui_impl_opengl3.h
        vendor/imgui/backends/imgui_impl_opengl3.cpp
)

file (GLOB IMGUIZMO_SRC "vendor/imguizmo/*.h" "vendor/imguizmo/*.cpp")

add_executable(marmalade
        vendor/glad/src/glad.c
        ${IMGUI_SRC}
        ${IMGUIZMO_SRC}
        vendor/imguifiledialog/ImGuiFileDialog.cpp
        vendor/imgui_markdown/imgui_markdown.h
)

add_subdirectory(include)
add_subdirectory(src)

if (NOT WIN32)
    add_dependencies(marmalade lua)
endif ()

# Copy static resources
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(marmalade PRIVATE
        glfw
        OpenGL::GL
        ${OPENGL_gl_LIBRARY}
        nlohmann_json::nlohmann_json
        glm::glm
        ${LUA_LIBRARIES}
        libgit2package
)

if (WIN32)
    target_link_libraries(marmalade PRIVATE Shcore.lib ws2_32.lib)
endif ()

target_include_directories(marmalade PRIVATE
        vendor/glad/include
        vendor/glfw-3.4/include
        vendor/imgui
        vendor/imguifiledialog
        vendor/imgui_markdown
        vendor/imguizmo
        vendor/glm/glm
        ${LUA_INCLUDE_DIR}
        vendor/luaaa
        vendor/stb
        vendor/libgit2/include
        vendor/iconfontcppheaders
        vendor/spdlog/include
)

target_include_directories(marmalade PRIVATE include)
target_include_directories(marmalade PRIVATE include)

target_link_directories(marmalade PRIVATE vendor/nativefiledialog/lib vendor/luaaa)
