# ImGui GUI
list(APPEND ENGINE_SRC_GUI
        components/util.h
        components/tableview.h
        components/selectablelistview.h

        imgui/about.cpp
        imgui/about.h
        imgui/editor.cpp
        imgui/editor.h
        imgui/editorviews.cpp
        imgui/editorviews.h
        imgui/editorviewstatusbar.cpp
        imgui/editorviewstatusbar.h
        imgui/scenehierarchy.cpp
        imgui/scenehierarchy.h
        imgui/topbar.cpp
        imgui/topbar.h
        imgui/projectwizard.cpp
        imgui/projectwizard.h
        imgui/preferences.cpp
        imgui/preferences.h
        imgui/details.cpp
        imgui/details.h
        imgui/settings.cpp
        imgui/settings.h
        imgui/projectbrowser.cpp
        imgui/projectbrowser.h
        imgui/packagemanager.cpp
        imgui/packagemanager.h
        imgui/log.cpp
        imgui/log.h
        imgui/welcomescreen.cpp
        imgui/welcomescreen.h

        editorviews.h
        editview.cpp
        editview.h
        gameview.cpp
        gameview.h
        stylemanager.cpp
        stylemanager.h
        window.h
        windowmanager.cpp
        windowmanager.h
)

target_sources(marmalade PRIVATE ${ENGINE_SRC_GUI})
