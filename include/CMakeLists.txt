# Engine API
list(APPEND ENGINE_API
        engineapi.h
        plugin.h
        projectbrowser.h
        versioncontrol.h

        imgui.inc
        interface.h
        wasm.h
)

target_sources(marmalade PRIVATE ${ENGINE_API})
