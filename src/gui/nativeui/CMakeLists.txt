list(APPEND ENGINE_SRC_NATIVE_UI
        util.h
        util_generic.cpp
        app.h
        msgbox.h
        window.h
        nativeobject.h
        splashscreen.h
        splashscreen.cpp
        menubar.h
)

if (UNIX AND NOT APPLE)
    list(APPEND ENGINE_SRC_NATIVE_UI
            linux/app.cpp
            linux/util.cpp
            linux/msgbox.cpp
            linux/window.cpp
            linux/menubar.cpp
    )
elseif (WIN32)
    list(APPEND ENGINE_SRC_NATIVE_UI
            win32/app.cpp
            win32/util.cpp
            win32/msgbox.cpp
            win32/window.cpp
            win32/menubar.cpp
    )
elseif (APPLE)
    list(APPEND ENGINE_SRC_NATIVE_UI
            darwin/util.mm
            darwin/DarwinObjWrapper.h
            darwin/AppDelegate.mm
            darwin/AppDelegate.h
            darwin/app.mm
            darwin/msgbox.mm
            darwin/window.mm
            darwin/menubar.mm
    )
endif ()

if (WIN32)
    list(APPEND ENGINE_SRC_NATIVE_UI
            ${CMAKE_SOURCE_DIR}/misc/resources/marmalade.exe.manifest
            ${CMAKE_BINARY_DIR}/marmalade.rc)
endif ()

if (APPLE)
    # Mark all files as objective-c++
    enable_language(OBJC)
    enable_language(OBJCXX)

    set_source_files_properties(${ENGINE_SRC_NATIVE_UI} PROPERTIES COMPILE_FLAGS "-x objective-c++")
    set_source_files_properties(${ENGINE_SRC_NATIVE_UI} PROPERTIES LANGUAGE OBJCXX)
endif ()


if (NOT TARGET marmalade_nativeui)
    add_library(marmalade_nativeui OBJECT ${ENGINE_SRC_NATIVE_UI})
    target_compile_definitions(marmalade_nativeui PRIVATE NATIVEUI_GLFW_WRAPPER NATIVEUI_ENABLE_IMGUI NATIVEUI_ENABLE_MENUBAR)
endif ()

if (NOT TARGET phroggy_nativeui)
    add_library(phroggy_nativeui OBJECT ${ENGINE_SRC_NATIVE_UI})
endif ()

target_link_libraries(marmalade_nativeui PUBLIC glfw nlohmann_json::nlohmann_json)
target_link_libraries(phroggy_nativeui PUBLIC glfw nlohmann_json::nlohmann_json)

target_include_directories(marmalade_nativeui PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui ${CMAKE_SOURCE_DIR}/vendor/iconfontcppheaders ${CMAKE_SOURCE_DIR}/vendor/spdlog/include)
target_include_directories(phroggy_nativeui PRIVATE ${CMAKE_SOURCE_DIR}/vendor/spdlog/include)

if (WIN32)
    target_link_libraries(marmalade_nativeui PUBLIC comctl32.lib)
    target_link_libraries(phroggy_nativeui PUBLIC comctl32.lib)
ENDIF ()

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)

    target_link_libraries(marmalade_nativeui PUBLIC ${COCOA_LIBRARY})
    target_link_libraries(phroggy_nativeui PUBLIC ${COCOA_LIBRARY})
ENDIF ()

if (UNIX AND NOT APPLE)
    target_link_libraries(marmalade_nativeui PUBLIC ${GTK3_LIBRARIES})
    target_link_libraries(phroggy_nativeui PUBLIC ${GTK3_LIBRARIES})
endif ()
