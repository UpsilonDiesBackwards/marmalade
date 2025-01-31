
#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "topbar.h"
#include "editorviews.h"
#include "scenehierarchy.h"

class Editor {
public:
    TopBar topBar;
    EditorViews editorViews;
    SceneHierarchy sceneHierarchy;

    // If `true` then ui element will be opened on startup
    bool showEditorViews = true;
    bool showSceneHeirarchy = true;

    void Render();
};


#endif //ENGINE_EDITOR_H
