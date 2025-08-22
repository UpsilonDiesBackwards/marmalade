// Marmalade - Lightweight Game Engine
// Copyright (C) 2025 Tayler Parsons
// Copyright (C) 2025 Ryan Bester
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

#include "../../../../include/ecs/components/rendering/modelrenderer.h"

#include <imgui.h>

#include <ImGuiFileDialog.h>
#include "../../../application/logger.h"
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include "graphics/renderable.h"
#include "scene/entity.h"

#include "../../../application/application.h"

void Marmalade::ECS::ModelRenderer::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    if (ImGui::Button("Load Model")) {
        IGFD::FileDialogConfig config;
        config.path = GET_APP.GetCurrentProject()->basePath.string();
        ImGuiFileDialog::Instance()->OpenDialog("ChooseModel", "Choose 3D Model", ".obj,.fbx,.dae,.3ds,.gltf,.glb", config);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseModel")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            modelPath = ImGuiFileDialog::Instance()->GetFilePathName();
            LoadModel(entity, modelPath);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void Marmalade::ECS::ModelRenderer::Apply(Entity* entity) {}

void Marmalade::ECS::ModelRenderer::Setup(Entity* entity) {
    if (modelPath.empty()) { return; }
    LoadModel(entity, modelPath);
}

nlohmann::json Marmalade::ECS::ModelRenderer::Serialize(const Entity* entity) {
    nlohmann::json j;

    j["file"] = modelPath;

    return j;
}

void Marmalade::ECS::ModelRenderer::Deserialize(nlohmann::json json, Entity* entity) {
    modelPath = json["file"].get<std::string>();

    LoadModel(entity, modelPath);
}

void Marmalade::ECS::ModelRenderer::LoadModel(Entity* entity, const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(),
                                             aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace |
                                             aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_FlipWindingOrder);

    if (!scene || !scene->mRootNode) {
        LOG_ERROR("Failed to load model {}, {}", path.c_str(), importer.GetErrorString());
        return;
    }

    ProcessNode(entity, scene->mRootNode, scene);
}

void Marmalade::ECS::ModelRenderer::Process(Entity* entity, aiMesh* mesh, const aiScene* scene) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Position
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Normals
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        // Texture Coords
        if (mesh->mTextureCoords[0]) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        } else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) { indices.push_back(face.mIndices[j]); }
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));

    glBindVertexArray(0);

    entity->renderable.VAO = VAO;
    entity->renderable.VBO = VBO;
    entity->renderable.EBO = EBO;

    entity->renderable.SetMeshData(vertices, indices);

    entity->renderable.Initialise();
}

void Marmalade::ECS::ModelRenderer::ProcessNode(Entity* entity, aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Process(entity, mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(entity, node->mChildren[i], scene);
    }
}