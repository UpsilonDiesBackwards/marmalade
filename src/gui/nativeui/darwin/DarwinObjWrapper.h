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

#ifndef MARMALADE_GUI_NATIVEUI_DARWINOBJWRAPPER_H
#define MARMALADE_GUI_NATIVEUI_DARWINOBJWRAPPER_H

namespace Marmalade::GUI::NativeUI {
    template<typename T>
    class DarwinObjWrapper {
    public:
        explicit DarwinObjWrapper(T object) {
            this->_object = object;
        };

        T GetObject() const {
            return _object;
        }
    private:
        T _object = nullptr;
    };
}

#endif
