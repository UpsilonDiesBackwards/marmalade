list(APPEND ENGINE_SRC_NATIVE_UI
        util.h
        util_generic.cpp
        app.h
        msgbox.h
        window.h
        nativeobject.h
)

if (UNIX AND NOT APPLE)
    list(APPEND ENGINE_SRC_NATIVE_UI
            linux/app.cpp
            linux/util.cpp
            linux/msgbox.cpp
            linux/window.cpp
    )
elseif (WIN32)
    list(APPEND ENGINE_SRC_NATIVE_UI
            win32/app.cpp
            win32/util.cpp
            win32/msgbox.cpp
            win32/window.cpp
    )
elseif (APPLE)
    list(APPEND ENGINE_SRC_NATIVE_UI

    )
endif ()

if (WIN32)
    configure_file(
            ${CMAKE_SOURCE_DIR}/misc/resources/marmalade.in.rc
            ${CMAKE_BINARY_DIR}/marmalade.rc)

    configure_file(
            ${CMAKE_SOURCE_DIR}/misc/resources/marmalade.exe.manifest
            ${CMAKE_BINARY_DIR}/marmalade.exe.manifest
    )
    list(APPEND ENGINE_SRC_NATIVE_UI
            ${CMAKE_SOURCE_DIR}/misc/resources/marmalade.exe.manifest
            ${CMAKE_BINARY_DIR}/marmalade.rc)

    set_target_properties(marmalade PROPERTIES
            VS_MANIFEST_FILE "${CMAKE_BINARY_DIR}/marmalade.exe.manifest"
    )

    if (MSVC)
        set_target_properties(marmalade PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    endif ()
elseif (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")

    find_library(COCOA_LIBRARY Cocoa)

    set_target_properties(marmalade PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME "Marmalade Engine"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_ICON_FILE icon.icns
            MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
            MACOSX_BUNDLE_COPYRIGHT "2025 Tayler Parsons"
            MACOSX_BUNDLE_GUI_IDENTIFIER com.diesbackwards.Marmalade
            XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
            XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
            XCODE_ATTRIBUTE_EXECUTABLE_NAME "Marmalade")
endif ()

if (WIN32)
    target_link_libraries(marmalade PRIVATE comctl32.lib)
ENDIF ()

if (APPLE)
    target_link_libraries(marmalade PRIVATE ${COCOA_LIBRARY})
ENDIF ()

if (UNIX AND NOT APPLE)
    target_link_libraries(marmalade PRIVATE ${GTK3_LIBRARIES})
endif ()

target_sources(marmalade PRIVATE ${ENGINE_SRC_NATIVE_UI})
