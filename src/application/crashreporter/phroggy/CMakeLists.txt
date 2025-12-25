cmake_minimum_required(VERSION 3.28)
project(marmalade_phroggy)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

add_compile_definitions(DISABLE_LOGGING)

add_compile_definitions($<IF:$<CONFIG:Debug>,DEBUG,>)
add_definitions(-DENGINE_BUILD)

if (APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)

    set(CMAKE_Swift_COMPILER /usr/bin/swiftc)
    enable_language(Swift)

    set_source_files_properties(main.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
    set_source_files_properties(main.cpp PROPERTIES LANGUAGE OBJCXX)
endif ()

add_subdirectory(../../../gui/nativeui ${CMAKE_CURRENT_BINARY_DIR}/nativeui)

add_executable(marmalade_phroggy WIN32 $<TARGET_OBJECTS:phroggy_nativeui>)

if (WIN32)
    target_sources(marmalade_phroggy PRIVATE main_win32.cpp)
elseif (UNIX AND NOT APPLE)
    target_sources(marmalade_phroggy PRIVATE main_linux.cpp)
elseif (APPLE)
    target_sources(marmalade_phroggy PRIVATE main_darwin.swift)
endif ()

set_target_properties(marmalade_phroggy PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
if (MSVC)
    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/phroggy.in.rc
            ${CMAKE_CURRENT_BINARY_DIR}/phroggy.rc)

    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/phroggy.exe.manifest
            ${CMAKE_CURRENT_BINARY_DIR}/phroggy.exe.manifest
    )
    target_sources(marmalade_phroggy PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/phroggy.exe.manifest
            ${CMAKE_CURRENT_BINARY_DIR}/phroggy.rc)

    set_target_properties(marmalade_phroggy PROPERTIES
            VS_MANIFEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/phroggy.exe.manifest"
    )

    set_target_properties(marmalade_phroggy PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

target_include_directories(marmalade_phroggy PRIVATE ../../../gui/nativeui)

target_link_libraries(marmalade_phroggy PRIVATE phroggy_nativeui)

if (MSVC)
    target_link_libraries(marmalade_phroggy PRIVATE Shcore.lib ws2_32.lib comctl32.lib)
endif ()

set_native_properties(marmalade_phroggy "Marmalade Crash Reporter" "1.0.0" "2025 Tayler Parsons" "com.diesbackwards.MarmaladePhroggy")
