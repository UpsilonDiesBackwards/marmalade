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

#ifndef MARMALADE_SEQUENCE_H
#define MARMALADE_SEQUENCE_H

#include "graphics/texture.h"

#include "../../src/application/config/config.h"
#include "animationframe.h"

#include <vector>
#include <glm/vec2.hpp>

#define ANIMATION_VERSION 1

namespace Marmalade::Animation {
    class AnimationSequence : Marmalade::Project::Assets::Asset, public Config<Marmalade::Animation::Frame> {
    public:
        std::string type{"Marmalade::Animation::Sequence"};
        int version{ ANIMATION_VERSION };

        std::string uuid{};
        std::string name{};
        float length = 1.0f;

        std::filesystem::path filePath;

        std::vector<Frame> frames{};
        bool loop{false};

        void Play();
        void Stop();

        explicit AnimationSequence(std::filesystem::path filePath, const std::string& name);

        void PrepareNewConfig() override;
    };
}


#endif//MARMALADE_ANIMATIONTIMELINE_H
