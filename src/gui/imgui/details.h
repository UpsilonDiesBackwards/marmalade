
#ifndef MARMALADE_DETAILS_H
#define MARMALADE_DETAILS_H

#include <caca_conio.h>
#include "../window.h"
#include "../../../include/archetypes/entity.h"

namespace Marmalade::GUI {
    class Details : public Window {
    public:
        Entity* inspectedEntity;

        void Draw() override;
    };
}

#endif//MARMALADE_DETAILS_H
