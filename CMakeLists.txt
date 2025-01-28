cmake_minimum_required(VERSION 3.28)
project(marmalade)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/json)
add_subdirectory(vendor/glm)

find_package(OpenGL REQUIRED)

file(GLOB IMGUI_SRC "vendor/imgui/*.h" "vendor/imgui/*.cpp")
list(APPEND IMGUI_SRC vendor/imgui/backends/imgui_impl_glfw.h vendor/imgui/backends/imgui_impl_glfw.cpp vendor/imgui/backends/imgui_impl_opengl3.h vendor/imgui/backends/imgui_impl_opengl3.cpp)

list(APPEND ENGINE_API include/archetypes/camera.h include/archetypes/entity.h
        include/graphics/renderable.h include/graphics/shader.h
        include/io/input.h include/io/inputmanager.h
        include/scene/scene.h include/scene/scenemanager.h
)

list(APPEND ENGINE_SRC src/gui/imgui/editor.cpp
        src/gui/imgui/editor.h
        src/gui/imgui/editorviews.cpp
        src/gui/imgui/editorviews.h
        src/gui/imgui/editorviewstatusbar.cpp
        src/gui/imgui/editorviewstatusbar.h
        src/gui/imgui/sceneheirarchy.cpp
        src/gui/imgui/sceneheirarchy.h
        src/gui/imgui/topbar.cpp
        src/gui/imgui/topbar.h
        src/gui/editorviews.h
        src/gui/editview.cpp
        src/gui/editview.h
        src/gui/framebuffer.cpp
        src/gui/framebuffer.h
        src/gui/gameview.cpp
        src/gui/gameview.h
        src/gui/imgui
        src/gui/stylemanager.cpp
        src/gui/stylemanager.h
)

list(APPEND ENGINE_SRC src/application/application.cpp src/application/application.h src/application/profiler.cpp src/application/profiler.h
        src/archetypes/camera.cpp src/archetypes/entity.cpp
        src/graphics/renderable.cpp src/graphics/shader.cpp
        src/io/input.cpp src/io/inputmanager.cpp
        src/scene/scene.cpp src/scene/scenemanager.cpp
)

add_executable(marmalade vendor/glad/src/glad.c ${IMGUI_SRC} vendor/imguifiledialog/ImGuiFileDialog.cpp
        ${ENGINE_API}
        ${ENGINE_SRC}
        src/main.cpp)

file(COPY vendor/imgui/misc/fonts/ProggyClean.ttf DESTINATION ${CMAKE_BINARY_DIR}/resources/fonts)
file(COPY vendor/imgui/misc/fonts/Roboto-Medium.ttf DESTINATION ${CMAKE_BINARY_DIR}/resources/fonts)

target_link_libraries(marmalade PRIVATE glfw OpenGL::GL ${OPENGL_gl_LIBRARY} nlohmann_json::nlohmann_json glm::glm)

if (WIN32)
    target_link_libraries(marmalade PRIVATE Shcore.lib ws2_32.lib)
endif ()

target_include_directories(marmalade PRIVATE vendor/glad/include vendor/glfw-3.4/include vendor/imgui vendor/imguifiledialog glm)
target_link_directories(marmalade PRIVATE vendor/nativefiledialog/lib)
