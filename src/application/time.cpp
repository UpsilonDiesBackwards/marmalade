/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "time.h"
#include "application.h"

#include <GLFW/glfw3.h>

Time::Time() : lastTime(0.0), deltaTime(0.0), fps(0), frameTime(0.0) {
}

void Time::Update() {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    fps = static_cast<int>(1.0 / deltaTime);

    frameTime = deltaTime * 1000.0f; // Convert to milliseconds
}

void Time::FixedUpdate() {
    if (Application::GetInstance().playState != PlayState::PlayState_STEP &&
        Application::GetInstance().playState != PlayState::PlayState_PAUSE) {

        _accumulator = fixedTimeStep;
    } else {
        _accumulator += fixedTimeStep;
    }


    while (_accumulator >= fixedTimeStep) { // We use an accumulator to prevent physic jitteriness
        _accumulator -= fixedTimeStep;
    }
}

int Time::GetCurrentFPS() const {
    return fps;
}

float Time::GetCurrentFrameTime() const {
    return frameTime;
}

double Time::GetDeltaTime() const {
    return deltaTime;
}

float Time::GetFixedDeltaTime() {
    return fixedTimeStep;
}
