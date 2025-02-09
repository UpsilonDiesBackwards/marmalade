
#include <spdlog/spdlog.h>
#include "mathematics/functions/sqrt.h"

namespace Marmalade::Mathematics {
    double sqrt_babylonian(double s) {
        if (s < 0) {
            spdlog::error("Can not get a square root of a negative number!");
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