#ifndef ENGINE_SCENEHIERARCHY_H
#define ENGINE_SCENEHIERARCHY_H

#include "../../../include/archetypes/entity.h"

class SceneHierarchy {
public:
    void Show();

private:
    Entity* _parent{nullptr};
    Entity* _selected{nullptr};
    bool _isCreatingEntityChild{false};
    bool _isDeleting{false};
    bool _isRenaming{false};

    void displayEntity(Entity* entity, int index);
    void showCreatePopup();
    void showRenamePopup();
    void showDeletePopup();
    void showContextMenu();
    void createEntity(const std::string& name);
};

#endif
