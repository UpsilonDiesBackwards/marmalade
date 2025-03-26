
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

#ifndef MARMALADE_ANIMATIONDRIVER_H
#define MARMALADE_ANIMATIONDRIVER_H

#include "sequence.h"
#include <memory>
#include <unordered_map>

#define ANIM_DRIVER_VERSION 1

namespace Marmalade::Animation {
    struct AnimationDriverData {
        std::string type{"Marmalade::Animation::Driver"};
        int version{ANIM_DRIVER_VERSION};

        std::string uuid{};
        std::string name{};

        std::unordered_map<std::string, std::string> sequences{};
        std::string currentSequence{};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationDriverData,
                                       version, uuid, name,
                                       sequences, currentSequence)

    class AnimationDriver : public Config<AnimationDriverData> {
    public:
        int version{ ANIM_DRIVER_VERSION };

        std::string uuid{};
        std::string name;

        std::unordered_map<std::string, std::shared_ptr<AnimationSequence>> animations{};
        std::shared_ptr<AnimationSequence> currentSequence = nullptr;

        bool isPlaying{false};
        float currentTime{0.0f};

        AnimationDriver() = default;
        explicit AnimationDriver(std::filesystem::path filePath, const std::string& name);

        void AddAnimation(const std::string& name, std::shared_ptr<AnimationSequence> sequence);

        void Play(const std::string& name);
        void Stop();

        void Update(float deltaTime);

        void Deserialise(const nlohmann::json& json);

        void PrepareNewConfig() override;
    };
}

#endif//MARMALADE_ANIMATIONDRIVER_H
