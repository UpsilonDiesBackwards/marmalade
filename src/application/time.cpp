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

/**
 * \brief Time constructor, sets defaults values
 */

Time::Time() : lastTime(0.0), deltaTime(0.0), fps(0), frameTime(0.0) {
}


void Time::Update() {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    fps = static_cast<int>(1.0 / deltaTime);

    frameTime = deltaTime * 1000.0f; // Convert to milliseconds

    // If enabled, save the project the game is NOT in play
    if (enableAutoSave && (!GET_APP.playState) != PlayState_STOP) { PollAutoSave(); }
}

/**
 * \brief If the engine play state is set to anything other than PlayState_STEP reset the accumulator
 * \brief if not, increase the accumulator by fixedTimeStep
 */
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

/**
 * \brief Returns the current frame rate
 * \return int fps
 */

int Time::GetCurrentFPS() const {
    return fps;
}

/**
 * \brief Returns the current frame time (time it takes to complete a frame in ms)
 * \return float frameTime
 */

float Time::GetCurrentFrameTime() const {
    return frameTime;
}

/**
 * \brief Returns the deltaTime
 * \return double deltaTime
 */

double Time::GetDeltaTime() const {
    return deltaTime;
}

/**
 * \brief Returns fixed deltaTime (deltaTime from fixed time step)
 * \return float fixedTimeStep
 */

float Time::GetFixedDeltaTime() {
    return fixedTimeStep;
}

void Time::PollAutoSave() {
    timeUntilNextAutosave -= deltaTime;
    timeSinceLastAutoSave += deltaTime;

    if (timeSinceLastAutoSave >= autoSaveInterval) {
        GET_APP.autoSave.SaveEngine();

        timeUntilNextAutosave = autoSaveInterval;
        timeSinceLastAutoSave = 0.0f;
    }
}
