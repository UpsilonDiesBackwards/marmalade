
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
