
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

#include "material.h"
#include "../application/util.h"

Marmalade::Material::Material::Material(std::filesystem::path filePath, const std::string& name, const std::string& uuid)
    : Config(filePath), name(name) {

    storedConfig.uuid = uuid;
    storedConfig.name = name;
    storedConfig.version = version;

    useGui = true;
}

void Marmalade::Material::Material::SetTexture(TextureMode mode, const std::string& filePath) {
    Texture* target = nullptr;

    switch (mode) {
        case TextureMode::Albedo: target = &albedo; break;
        case TextureMode::Normal: target = &normal; break;
        case TextureMode::Specular: target = &specular; break;
        case TextureMode::Roughness: target = &roughness; break;
    }

    if (!target) return;

    if (target->id != 0) { // If old texture exists, remove it
        glDeleteTextures(1, &target->id);
    }

    target->filePath = filePath;
    target->id = ::Texture::LoadTexture(filePath, target->settings);

    UpdateTextureSettings(*target);
}

void Marmalade::Material::Material::UpdateTextureSettings(Texture& texture) {
    if (texture.id == 0) return;

    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.settings.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.settings.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.settings.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.settings.magFilter);
}

GLuint Marmalade::Material::Material::GetTexture(TextureMode mode) const {
    switch (mode) {
        case TextureMode::Albedo:    return albedo.id;
        case TextureMode::Normal:    return normal.id;
        case TextureMode::Specular:  return specular.id;
        case TextureMode::Roughness: return roughness.id;
    }
    return 0;
}

void Marmalade::Material::Material::Deserialise(const nlohmann::json& json) {
    storedConfig = json.get<MaterialData>();
}

void Marmalade::Material::Material::PrepareNewConfig() {
    storedConfig.albedo = albedo;
    storedConfig.normal = normal;
    storedConfig.specular = specular;
    storedConfig.roughness = roughness;

    Config::PrepareNewConfig();
}