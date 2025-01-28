
#ifndef ENGINE_PROFILER_H
#define ENGINE_PROFILER_H


class Profiler {
public:
    Profiler();

    void Update();

    int GetCurrentFPS() const;
    float GetCurrentFrameTime() const;
    double GetDeltaTime() const;

private:
    double lastTime;
    double deltaTime;
    int fps;
    float frameTime;
};


#endif //ENGINE_PROFILER_H
