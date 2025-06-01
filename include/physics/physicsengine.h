
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

#ifndef MARMALADE_PHYSICSENGINE_H
#define MARMALADE_PHYSICSENGINE_H

namespace Marmalade::Physics {
    /**
     * \class PhysicsEngine
     * \brief Abstract base class for physics engine implementations.
     *
     * Defines the interface for updating the physics simulation.
     */
    class PhysicsEngine {
    public:
        /**
         * \brief Update the physics simulation.
         *
         * This method must be overridden by derived classes to perform
         * physics calculations and state updates.
         */
        virtual void Update() = 0;
        virtual ~PhysicsEngine() = default;
    };
}

#endif //MARMALADE_PHYSICSENGINE_H
