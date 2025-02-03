#ifndef MARMALADE_WINDOW_H
#define MARMALADE_WINDOW_H

namespace Marmalade::GUI {
    class Window {
    public:
        bool visible{false};

        Window() = default;
        explicit Window(bool visible) : visible(visible) {};
        virtual ~Window() = default;

        virtual void Draw() = 0;

        inline void Show() {
            if (!visible) return;
            Draw();
        }

        inline void ToggleWindow() {
            visible = !visible;
        }
    };
}

#endif
