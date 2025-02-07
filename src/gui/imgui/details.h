
#ifndef MARMALADE_DETAILS_H
#define MARMALADE_DETAILS_H

#include "../window.h"
#include "../../../include/archetypes/entity.h"

namespace Marmalade::GUI {
    class Details : public Window {
    public:
        Entity* inspectedEntity;

        void Draw() override;
    private:
        Component* _selectedComponent;

        bool _isAddingComponent{false};
        bool _isRemovingComponent{false};

        void ShowAddPopup();
        void ShowRemovePopup();
    };
}

#endif//MARMALADE_DETAILS_H
