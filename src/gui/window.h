#ifndef MARMALADE_GUI_WINDOW_H
#define MARMALADE_GUI_WINDOW_H

namespace Marmalade::GUI {
    /**
     * \brief Abstract class to control an ImGui window.
     */
    class Window {
    public:
        /**
         * \brief The visibility state of the window.
         */
        bool visible{false};

        Window() = default;
        explicit Window(bool visible) : visible(visible) {};
        virtual ~Window() = default;

        /**
         * \brief Draws the window.
         *
         * Will only be called if Window#visible is true.
         * This should contain the `ImGui::Begin` function with `&visible` as the `p_open` parameter.
         */
        virtual void Draw() = 0;

        /**
         * \brief Shows the window if should be shown.
         *
         * Wrapper for the Window#Draw method, but only calls if Window#visible is true.
         */
        inline void Show() {
            if (!visible) return;
            Draw();
        }

        /**
         * \brief Toggles the visibility of the window.
         */
        inline void ToggleWindow() {
            visible = !visible;
        }
    };
}

#endif
