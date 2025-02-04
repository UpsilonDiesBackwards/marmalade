
#ifndef ENGINE_EDITVIEW_H
#define ENGINE_EDITVIEW_H


#include "framebuffer.h"
#include "imgui.h"

class EditView {
public:
    EditView(int width, int height);
    ~EditView();

    void Render();
    void Resize(int width, int height);
private:
    Framebuffer* framebuffer;
    int width, height;
    ImVec2 imageMin, imageMax;
};


#endif //ENGINE_EDITVIEW_H
