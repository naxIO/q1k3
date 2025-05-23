#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>
#include <algorithm>

inline float clamp(float v, float min, float max) {
    return v < min ? min : (v > max ? max : v);
}

inline float scale(float v, float in_min, float in_max, float out_min, float out_max) {
    return out_min + (out_max - out_min) * ((v - in_min) / (in_max - in_min));
}

inline float anglemod(float r) {
    return std::atan2(std::sin(r), std::cos(r));
}

#endif // MATH_UTILS_H