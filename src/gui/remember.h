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

#ifndef MARMALADE_GUI_REMEMBER_H
#define MARMALADE_GUI_REMEMBER_H

namespace Marmalade::GUI {

    /**
     * \brief Simple class to cache UI data.
     *
     * Store as a static variable in UI code.
     *
     * Example usage:
     * \code
     * static auto data = Remember(IntensiveGetDataFunction);
     *
     * for (const auto& item : data.get()) {
     *     ImGui::Text("%s", item);
     * }
     * \endcode
     *
     * \tparam Provider The provider function type
     */
    template<typename Provider>
    class Remember {
    public:
        using T = std::invoke_result_t<Provider>;

        explicit Remember(Provider provider) : _provider(provider) {};

        T& get() {
            if (!_cache.has_value()) {
                _cache = _provider();
            }
            return *_cache;
        }

        /**
         * \brief Clears the cache and loads fresh data.
         */
        void reset() {
            _cache.reset();
        }

        T* operator->() {
            return &get();
        }

        T& operator*() {
            return get();
        }

    private:
        Provider _provider;
        std::optional<T> _cache{};
    };

    template<typename Provider>
    Remember(Provider) -> Remember<Provider>;

}

#endif
