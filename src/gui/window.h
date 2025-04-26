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

#ifndef MARMALADE_GUI_WINDOW_H
#define MARMALADE_GUI_WINDOW_H

#define WINDOW_BEGIN(name, flags)        \
    ImGui::Begin(name, &visible, flags); \
    {                                    \
        const char* _window_name = name; \
        Marmalade::InterfaceApiImpl::CallBeginHooks(name);

#define WINDOW_END()                                         \
    Marmalade::InterfaceApiImpl::CallEndHooks(_window_name); \
    ImGui::End();                                            \
    }

#define WINDOW_BEGIN_MODAL(name, flags)                  \
    if (visible) {                                       \
        ImGui::OpenPopup(name);                          \
    }                                                    \
    if (ImGui::BeginPopupModal(name, &visible, flags)) { \
        const char* _window_name = name;                 \
        Marmalade::InterfaceApiImpl::CallBeginHooks(name);

#define WINDOW_END_MODAL()                                   \
    Marmalade::InterfaceApiImpl::CallEndHooks(_window_name); \
    ImGui::EndPopup();                                       \
    }

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
