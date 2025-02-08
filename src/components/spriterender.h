
#ifndef MARMALADE_SPRITERENDER_H
#define MARMALADE_SPRITERENDER_H

#include "../../include/archetypes/component.h"

class SpriteRender : public Component {
public:
    void Display(Entity* entity) override;
    void Apply(Entity* entity) override;

    SpriteRender() { name = "Sprite Render"; }

    REGISTER_COMPONENT(SpriteRender)
};


#endif//MARMALADE_SPRITERENDER_H
