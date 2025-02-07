#include "spriterender.h"

#include <imgui.h>
#include "../../include/archetypes/entity.h"

void SpriteRender::Display(Entity* entity) {
    ImGui::Text(name.c_str());

    ImGui::Image(ImTextureID(entity->renderable.GetTexture()), ImVec2(256, 256));

}

void SpriteRender::Apply(Entity* entity) {

}
