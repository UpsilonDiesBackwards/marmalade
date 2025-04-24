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

#ifndef MARMALADE_GUI_COMPONENTS_UTIL
#define MARMALADE_GUI_COMPONENTS_UTIL

#include <type_traits>
#include <memory>

namespace Marmalade::GUI::Components {
    /**
     * @brief Type trait to check if a container `L` is a sequence container of type `T`.
     *
     * This trait enables a type only if `L` has:
     * - `value_type` equal to `T`
     * - `allocator_type` equal to `std::allocator<T>`
     *
     * @tparam T Expected element type.
     * @tparam L Container type to check.
     */
    template<typename T, typename L>
    using IsSequenceContainer = std::enable_if_t<
            std::is_same_v<typename L::value_type, T> &&
            std::is_same_v<typename L::allocator_type, std::allocator<T>>>;

    /**
     * @brief Primary template for checking if a type has a `SetSelected` method.
     *
     * Defaults to `false_type` if `T` does not have a `SetSelected(bool)` method.
     *
     * @tparam T Type to check.
     * @tparam Enable SFINAE parameter (default `void`).
     */
    template<typename T, typename Enable = void>
    struct HasSetSelected : std::false_type {};

    /**
     * @brief Specialisation of `HasSetSelected` for types that have `SetSelected(bool)`.
     *
     * Uses `std::void_t` to detect if `T` has a member function with the signature:
     * `void SetSelected(bool)`.
     *
     * @tparam T Type to check.
     */
    template<typename T>
    struct HasSetSelected<T, std::void_t<decltype(std::declval<T>().SetSelected(true))>> : std::true_type {};

    /**
     * @brief Primary template for checking if a type has an `IsSelected` method.
     *
     * Defaults to `false_type` if `T` does not have an `IsSelected()` method.
     *
     * @tparam T Type to check.
     * @tparam Enable SFINAE parameter (default `void`).
     */
    template<typename, typename = void>
    struct HasIsSelected : std::false_type {};

    /**
     * @brief Specialisation of HasIsSelected for types that have `IsSelected()`.
     *
     * Uses `std::void_t` to detect if `T` has a member function with the signature:
     * `bool IsSelected()`. The return type must be exactly `bool`.
     *
     * @tparam T Type to check.
     */
    template<typename T>
    struct HasIsSelected<T, std::void_t<decltype(std::declval<T>().IsSelected())>> : std::is_same<decltype(std::declval<T>().IsSelected()), bool> {};

    /**
     * @brief Trait to check if a type is selectable.
     *
     * A type `T` is selectable if it has both:
     * - `void SetSelected(bool)`
     * - `bool IsSelected()`
     *
     * Uses `std::conjunction` to ensure both conditions are met.
     *
     * @tparam T Type to check.
     */
    template<typename T>
    struct IsSelectable : std::conjunction<
                                  HasSetSelected<T>,
                                  HasIsSelected<T>> {};

    /**
     * @brief Type alias for enabling template specialisation based on `IsSelectable<T>`.
     *
     * This alias uses `std::enable_if_t` to enable a template only if the type `T`
     * is **selectable**, i.e., it satisfies the `IsSelectable<T>` type trait.
     * The type trait `IsSelectable<T>` checks if `T` has both `SetSelected(bool)` and `IsSelected()` methods.
     *
     * @tparam T The type being checked for selectability.
     *
     * If `T` satisfies the conditions of `IsSelectable<T>`, the alias will be resolved to `int`,
     * which can then be used to enable or specialize a template. If `T` does not satisfy the conditions,
     * the alias will not be valid, effectively preventing template instantiation.
     */
    template<typename T>
    using EnableIfSelectable = std::enable_if_t<IsSelectable<T>::value, int>;
}

#endif
