# ImGui GUI
list(APPEND ENGINE_SRC_GUI
        components/util.h
        components/tableview.h
        components/selectablelistview.h
        components/markdownparser.cpp
        components/markdownparser.h

        dialogs/about.cpp
        dialogs/about.h
        dialogs/addcomponent.cpp
        dialogs/addcomponent.h
        dialogs/configerror.cpp
        dialogs/configerror.h

        editor/details.cpp
        editor/details.h
        editor/editor.cpp
        editor/editor.h
        editor/editorviewport.cpp
        editor/editorviewport.h
        editor/gameviewport.cpp
        editor/gameviewport.h
        editor/projectbrowser.cpp
        editor/projectbrowser.h
        editor/scenehierarchy.cpp
        editor/scenehierarchy.h
        editor/topbar.cpp
        editor/topbar.h
        editor/animationtimeline.cpp
        editor/animationtimeline.h

        log/log.cpp
        log/log.h

        settings/preferences.cpp
        settings/preferences.h
        settings/settings.cpp
        settings/settings.h

        windows/packagemanager.cpp
        windows/packagemanager.h
        windows/stylemanager.cpp
        windows/stylemanager.h
        windows/welcomescreen.cpp
        windows/welcomescreen.h

        wizards/projectwizard.cpp
        wizards/projectwizard.h

        editor.cpp
        editor.h
        window.h
        windowmanager.cpp
        windowmanager.h
        fontmanager.cpp
        fontmanager.h
)

target_sources(marmalade PRIVATE ${ENGINE_SRC_GUI})
