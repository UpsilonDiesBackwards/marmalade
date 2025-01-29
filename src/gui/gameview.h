
#ifndef ENGINE_GAMEVIEW_H
#define ENGINE_GAMEVIEW_H


#include "framebuffer.h"
#include "imgui.h"

class GameView {
public:
    GameView(int width, int height);
    ~GameView();

    void Render();
    void Resize(int width, int height);

    void RunInput();
private:
    Framebuffer framebuffer;
    int width, height;
    ImVec2 imageMin, imageMax;
};


#endif //ENGINE_GAMEVIEW_H
