// Marmalade - Lightweight Game Engine
// Copyright (C) 2025 Tayler Parsons
// Copyright (C) 2025 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PHYSICSENGINE2D_H
#define PHYSICSENGINE2D_H

#include "../physicsengine.h"
#include "physics/collisionevent.h"

#include "scene/entity.h"

namespace Marmalade::Physics {
    class PhysicsEngine2D : PhysicsEngine {
    public:
        void Update() override;
        ~PhysicsEngine2D() override;

    private:
        void EulerIntegration();
        void BroadPhase();
        void NarrowPhase();
        void Resolve();

        void ResolveCollision(CollisionEvent event);
        void PositionCorrection(CollisionEvent event);

        CollisionEvent DetectCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    };
}

#endif //PHYSICSENGINE2D_H