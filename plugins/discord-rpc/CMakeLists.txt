cmake_minimum_required(VERSION 3.28.3)
project(discord_rpc_plugin CXX)

set(CMAKE_CXX_STANDARD 20)

# Set architecture here
set(DISCORD_GAME_SDK_ARCH "x86_64")

if (WIN32)
    set(STATIC_EXTENSION ".dll.lib")
elseif (APPLE)
    set(STATIC_EXTENSION ".dylib")
else ()
    set(STATIC_EXTENSION ".so")
endif ()

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

include_directories(../../include "${CMAKE_CURRENT_SOURCE_DIR}/vendor/discord_game_sdk/c")

add_library(discord_rpc_plugin SHARED discordrpc.cpp discordrpc.h)

target_link_libraries(discord_rpc_plugin PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/discord_game_sdk/lib/${DISCORD_GAME_SDK_ARCH}/discord_game_sdk${STATIC_EXTENSION}")
