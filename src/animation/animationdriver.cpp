
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

#include "animation/animationdriver.h"

Marmalade::Animation::AnimationDriver::AnimationDriver(std::filesystem::path filePath, const std::string& name) :
    Config(filePath), name(name) {

    useGui = true;
}

void Marmalade::Animation::AnimationDriver::AddAnimation(const std::string& name, std::shared_ptr<AnimationSequence> sequence) {
    animations[name] = std::move(sequence);
}

void Marmalade::Animation::AnimationDriver::Play(const std::string& name) {
    if (animations.find(name) != animations.end()) {
        currentSequence = animations[name];
        currentTime = 0.0f;
        isPlaying = true;
    }
}

void Marmalade::Animation::AnimationDriver::Stop() {
    isPlaying = false;
    currentSequence = nullptr;
}

void Marmalade::Animation::AnimationDriver::Update(float deltaTime) {
    if (!isPlaying || !currentSequence) { return; }

    currentTime += deltaTime;
    if (currentTime >= currentSequence->length) {
        currentTime = 0.0f;
    } else {
        Stop();
    }
}

void Marmalade::Animation::AnimationDriver::Deserialise(const nlohmann::json& json) {
    storedConfig = json.get<AnimationDriverData>();

    animations.clear();

    for (const auto& [name, pathStr] : storedConfig.sequences) {
        std::filesystem::path path = pathStr;
        if (std::filesystem::exists(path)) {
            auto anim = std::make_shared<AnimationSequence>(path, name);
            animations[name] = anim;
            LOG_DEBUG("Loaded animation '{}' from '{}'", name, path.string());
        } else {
            LOG_WARN("Animation '{}' path '{}' does not exist!", name, path.string());
        }
    }

    if (!storedConfig.currentSequence.empty() && animations.count(storedConfig.currentSequence)) {
        currentSequence = animations[storedConfig.currentSequence];
        LOG_DEBUG("Set '{}' as the current sequence.", storedConfig.currentSequence);
    } else {
        currentSequence = nullptr;
        LOG_WARN("No valid current sequence found.");
    }
}


void Marmalade::Animation::AnimationDriver::PrepareNewConfig() {
    storedConfig.uuid = uuid;
    storedConfig.name = name;

    Config::PrepareNewConfig();
}