
#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "topbar.h"
#include "editorviews.h"
#include "scenehierarchy.h"
#include "details.h"

class Editor {
public:
    Marmalade::GUI::TopBar topBar;
    EditorViews editorViews;
    SceneHierarchy sceneHierarchy;
    Marmalade::GUI::Details details;

    // If `true` then ui element will be opened on startup
    bool showEditorViews = true;
    bool showSceneHeirarchy = true;
    bool showDetails = true;

    void Render();
};


#endif //ENGINE_EDITOR_H
