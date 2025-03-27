
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

#include <ecs/components/physics2d/colliderdata.h>
#include <ecs/components/physics2d/colliderbase.h>

Marmalade::ECS::ColliderInfo Marmalade::ECS::GetColliderInfo(const ColliderBase* collider) {
    ColliderInfo info;

    std::visit([&](auto&& colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            info.size = colliderData.size;
            info.offset = colliderData.offset;
        }
        if constexpr (std::is_same_v<T, OBBDataBox>) {
            info.rotation = colliderData.rotation;
            info.center = colliderData.c;
            info.axes[0] = colliderData.u[0];
            info.axes[1] = colliderData.u[1];
            info.halfWidth = colliderData.e;
        }
        if constexpr (std::is_same_v<T, DataCircle>) {
            info.radius = colliderData.radius;
            info.offset = colliderData.offset;
        }
    }, collider->data);

    return info;
}
