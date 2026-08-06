// Marmalade - Lightweight Game Engine
// Copyright (C) 2026 Tayler Parsons
// Copyright (C) 2026 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MARMALADE_MODELRENDERER_H
#define MARMALADE_MODELRENDERER_H

#include "../../component.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace Marmalade::ECS {
    class ModelRenderer : public Component {
    public:
        std::string modelPath;

        ModelRenderer() {
            name = "Model Renderer";
            categories = {"Rendering", "3D"};
            description = "Loads and renders a 3D model using Assimp";
        }

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

    private:
        void LoadModel(Entity* entity, const std::string& path);
        void Process(Entity* entity, aiMesh* mesh, const aiScene* scene);
        void ProcessNode(Entity* entity, aiNode* node, const aiScene* scene);
    };

    REGISTER_COMPONENT(ModelRenderer);
}

#endif //MARMALADE_MODELRENDERER_H
