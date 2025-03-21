
/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_ANIMATIONPLAYER_H
#define MARMALADE_ANIMATIONPLAYER_H

#include "ecs/component.h"

#include "animation/animation.h"

namespace Marmalade::ECS {
    class AnimationPlayer : public Component {
    public:
#define ANIMATION_PLAYER_CTOR_BODY         \
    name = "Animation Player";             \
    isMutable = true;                      \
    allowMultiple = false;                 \
    categories = {"Animation"};            \
    description =                          \
            "Animation Player Component\n" \
            "Allows 2D animation sequencing and playing";

        // MSVC is a very broken compiler and requires the constructor to be in the source file. But only for this component.
        // All the other components are fine. And we want the definition to be in the header so this macro helps out.
#ifdef _MSC_VER
        AnimationPlayer();
#else
        AnimationPlayer(){
                ANIMATION_PLAYER_CTOR_BODY}
#endif

        std::unique_ptr<Marmalade::Animation::AnimationSequence> animation = nullptr;

        bool _isCreatingSequence{false};

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        void showCreatePopup();
    };

    REGISTER_COMPONENT(AnimationPlayer);
}

#endif
