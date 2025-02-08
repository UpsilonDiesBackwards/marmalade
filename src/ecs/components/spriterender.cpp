#include "../../../include/ecs/components/spriterender.h"

#include "imgui.h"
#include "../../../include/scene/entity.h"

void SpriteRender::Display(Entity* entity) {
    ImGui::Text(name.c_str());

    ImGui::Image(ImTextureID(entity->renderable.GetTexture()), ImVec2(256, 256));

}

void SpriteRender::Apply(Entity* entity) {

}

bool SpriteRender::_registered = SpriteRender::Register();
