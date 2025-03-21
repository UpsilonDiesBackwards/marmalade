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

#ifndef MARMALADE_ANIMATION_H
#define MARMALADE_ANIMATION_H

#include "graphics/texture.h"

#include "../../src/application/config/config.h"

#include <vector>
#include <glm/vec2.hpp>

#define ANIMATION_VERSION 1

namespace Marmalade::Animation {
    struct Frame {
        int version{ ANIMATION_VERSION };

        std::string type{"Marmalade::Animation"};

        Texture sprite;
        float speed;
        glm::vec2 offset;

        // TODO: Animation events
    };

    class AnimationSequence : public Config<Marmalade::Animation::Frame> {
    public:
        std::string uuid{};

        std::string name{};
        float length;

        std::vector<Frame> frames{};
        bool loop;

        void Play();
        void Stop();

        explicit AnimationSequence(std::filesystem::path filePath, const std::string& name);
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::Animation::AnimationSequence, name, frames, loop)
}


#endif//MARMALADE_ANIMATIONTIMELINE_H
