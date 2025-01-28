
#include "../../gui/imgui/editor.h"

void Editor::Render() {
    // Persistent UI
    topBar.Show();

    if (showEditorViews) {
        editorViews.Show();
    }

    if (showSceneHeirarchy) {
        sceneHeirarchy.Show();
    }
}
