
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

#ifndef MARMALADE_MATERIAL_H
#define MARMALADE_MATERIAL_H

#include "../../src/application/config/config.h"
#include "../../src/project/assetregistry.h"

#include <glm/glm.hpp>

#include "texture.h"

#include "imgui.h"

#include <vector>

#define MATERIAL_VERSION 1

namespace Marmalade::Material {
    struct TextureSettings {
        int wrapS = GL_REPEAT;
        int wrapT = GL_REPEAT;
        int minFilter = GL_LINEAR_MIPMAP_LINEAR;
        int magFilter = GL_LINEAR;

        float uvScaleX = 1.0f;
        float uvScaleY = 1.0f;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextureSettings,
                                       wrapS, wrapT, minFilter, magFilter, uvScaleX, uvScaleY);

    enum TextureMode {
        Albedo,
        Normal,
        Specular,
        Roughness
    };

    struct Texture {
        GLuint id = 0;
        std::string filePath;
        TextureMode mode = Albedo;
        TextureSettings settings;

        Texture() = default;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Texture,
                                       id, filePath, mode, settings);

    struct MaterialData {
        std::string type{"Marmalade::Material"};
        int version{MATERIAL_VERSION};

        std::string name{};
        std::string uuid{};

        Texture albedo{};
        Texture normal{};
        Texture specular{};
        Texture roughness{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MaterialData,
                                       type, version, name, uuid,
                                       albedo, normal, specular, roughness);

    class Material : public Config<MaterialData> {
    public:
        int version{ MATERIAL_VERSION };

        std::string uuid{};
        std::string name;

        Texture albedo{};
        Texture normal{};
        Texture specular{};
        Texture roughness{};

        void SetTexture(TextureMode mode, const std::string& filePath);
        void UpdateTextureSettings(Texture& texture);
        GLuint GetTexture(TextureMode mode) const;

        Material() = default;
        explicit Material(std::filesystem::path filePath, const std::string& name, const std::string& uuid);

        void Deserialise(const nlohmann::json& json);

        void PrepareNewConfig() override;
    };
}

#endif//MARMALADE_MATERIAL_H
