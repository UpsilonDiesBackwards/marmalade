cmake_minimum_required(VERSION 3.28)
project(marmalade_installer)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

add_compile_definitions(DISABLE_LOGGING)

add_compile_definitions($<IF:$<CONFIG:Debug>,DEBUG,>)
add_definitions(-DENGINE_BUILD)

if (NOT WIN32)
    message(FATAL_ERROR "Marmalade Installer is only for Windows systems")
endif ()

add_executable(marmalade_installer WIN32
        src/main.cpp
        src/resource.h
        ../../src/application/integration.h
        ../../src/application/integration.cpp
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/marmaladeinstaller.in.rc
        ${CMAKE_CURRENT_BINARY_DIR}/marmaladeinstaller.rc)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/marmaladeinstaller.exe.manifest
        ${CMAKE_CURRENT_BINARY_DIR}/marmaladeinstaller.exe.manifest
)
target_sources(marmalade_installer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/marmaladeinstaller.exe.manifest
        ${CMAKE_CURRENT_BINARY_DIR}/marmaladeinstaller.rc)

set_target_properties(marmalade_installer PROPERTIES
        VS_MANIFEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/marmaladeinstaller.exe.manifest"
)

if (MSVC)
    set_target_properties(marmalade_installer PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

target_include_directories(marmalade_installer PRIVATE ../../src ../../vendor/zip/src)

target_link_libraries(marmalade_installer PRIVATE Shcore.lib ws2_32.lib comctl32.lib zip::zip)

# Compress marmalade publish directory
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources/marmalade.zip
        COMMAND "C:/Program Files/7-Zip/7z.exe" a -tzip "${CMAKE_CURRENT_BINARY_DIR}/resources/marmalade.zip" "${CMAKE_BINARY_DIR}/publish/*"
        DEPENDS ${CMAKE_BINARY_DIR}/publish
        COMMENT "Compressing publish directory"
)

add_custom_target(compress_publish ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources/marmalade.zip
)

add_dependencies(compress_publish marmalade_publish)
add_dependencies(marmalade_installer compress_publish)
