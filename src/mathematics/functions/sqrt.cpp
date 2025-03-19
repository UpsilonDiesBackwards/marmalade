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

#include "../../application/logger.h"

#include <mathematics/functions/sqrt.h>

namespace Marmalade::Mathematics {
    double sqrt_babylonian(double s) {
        if (s < 0) {
            LOG_ERROR("Can not get a square root of a negative number!");
        } else if (s == 0) {
            return 0;
        }

        double x = s / 2.0;
        while (std::abs(x * x - s) > 1e-6) {// 1e-6 is epsilon
            x = (x + s / x) / 2.0;
        }
        return x;
    }
}
