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

#ifndef MARMALADE_LIGHT2D_H
#define MARMALADE_LIGHT2D_H

#include "../../component.h"
#include "glad/glad.h"
#include "../../../graphics/material.h"
#include "../../../graphics/shader.h"
#include "../../../graphics/components/billboardtexture.h"

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

namespace Marmalade::ECS {
    class Light2D : public Component {
    public:
#define LIGHT2D_CTOR_BODY                     \
    name = "Light2D";                         \
    isMutable = true;                         \
    allowMultiple = false;                    \
    categories = {"Lighting"};                \
    description =                             \
            "Adds a 2D light to the entity\n" \
            "Uses the Transform's position and scale, and will affect the appearance of nearby entities. CURRENTLY ONLY A POINT LIGHT";

        // Same pattern as animationplayer
#ifdef _MSC_VER
        Light2D();
#else
        Light2D(){
                LIGHT2D_CTOR_BODY}
#endif

        glm::vec2 position = glm::vec3(0.0f);

        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
        float radius = 5.0f;
        float attenuation = 1.0f;

        bool showingBounds = true;

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        glm::vec2 GetPosition();

        void ShowBounds(Entity* entity);
        float WorldRadiusToScreenScale(float radius);

    private:
        static Shader* billboardShader;
        static BillboardTexture* lightBillboard;
    };

    REGISTER_COMPONENT(Light2D);
}

#endif//MARMALADE_LIGHT2D_H
