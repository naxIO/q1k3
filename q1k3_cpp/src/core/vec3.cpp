#include "vec3.h"

vec3 vec3_rotate_x(const vec3& p, float rad) {
    float c = std::cos(rad);
    float s = std::sin(rad);
    return vec3(p.x, p.y * c - p.z * s, p.y * s + p.z * c);
}

vec3 vec3_rotate_y(const vec3& p, float rad) {
    float c = std::cos(rad);
    float s = std::sin(rad);
    return vec3(p.z * s + p.x * c, p.y, p.z * c - p.x * s);
}

vec3 vec3_rotate_yaw_pitch(const vec3& p, float yaw, float pitch) {
    return vec3_rotate_y(vec3_rotate_x(p, pitch), yaw);
}

float vec3_2d_angle(const vec3& a, const vec3& b) {
    return std::atan2(b.x - a.x, b.z - a.z);
}