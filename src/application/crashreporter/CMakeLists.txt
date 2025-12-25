add_subdirectory(phroggy)

list(APPEND ENGINE_SRC_CRASHREPORTER
    crashreporter.h
)

if (UNIX AND NOT APPLE)
    list(APPEND ENGINE_SRC_CRASHREPORTER
        crashreporter_linux.cpp
    )
elseif (WIN32)
    list(APPEND ENGINE_SRC_CRASHREPORTER
        crashreporter_win32.cpp
    )
elseif (APPLE)
    list(APPEND ENGINE_SRC_CRASHREPORTER
        crashreporter_darwin.cpp
    )
endif ()

target_sources(marmalade PRIVATE ${ENGINE_SRC_CRASHREPORTER})
