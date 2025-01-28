
#include "profiler.h"
#include "GLFW/glfw3.h"

Profiler::Profiler() : lastTime(0.0), deltaTime(0.0), fps(0), frameTime(0.0) {
}

void Profiler::Update() {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    fps = static_cast<int>(1.0 / deltaTime);

    frameTime = deltaTime * 1000.0f; // Convert to milliseconds
}

int Profiler::GetCurrentFPS() const {
    return fps;
}

float Profiler::GetCurrentFrameTime() const {
    return frameTime;
}

double Profiler::GetDeltaTime() const {
    return deltaTime;
}