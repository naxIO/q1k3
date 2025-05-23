#ifndef VEC3_H
#define VEC3_H

#include <cmath>

struct vec3 {
    float x, y, z;
    
    vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    
    vec3 operator+(const vec3& b) const { return vec3(x + b.x, y + b.y, z + b.z); }
    vec3 operator-(const vec3& b) const { return vec3(x - b.x, y - b.y, z - b.z); }
    vec3 operator*(const vec3& b) const { return vec3(x * b.x, y * b.y, z * b.z); }
    vec3 operator*(float b) const { return vec3(x * b, y * b, z * b); }
    
    vec3& operator+=(const vec3& b) { x += b.x; y += b.y; z += b.z; return *this; }
    vec3& operator-=(const vec3& b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
    vec3& operator*=(float b) { x *= b; y *= b; z *= b; return *this; }
};

inline vec3 vec3_clone(const vec3& a) { return vec3(a.x, a.y, a.z); }
inline float vec3_length(const vec3& a) { return std::hypot(std::hypot(a.x, a.y), a.z); }
inline float vec3_dist(const vec3& a, const vec3& b) { return vec3_length(a - b); }
inline float vec3_dot(const vec3& a, const vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline vec3 vec3_add(const vec3& a, const vec3& b) { return a + b; }
inline vec3 vec3_sub(const vec3& a, const vec3& b) { return a - b; }
inline vec3 vec3_mul(const vec3& a, const vec3& b) { return a * b; }
inline vec3 vec3_mulf(const vec3& a, float b) { return a * b; }
inline vec3 vec3_cross(const vec3& a, const vec3& b) {
    return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
inline vec3 vec3_normalize(const vec3& v) { 
    float len = vec3_length(v);
    return len > 0 ? v * (1.0f / len) : vec3();
}
inline vec3 vec3_face_normal(const vec3& v0, const vec3& v1, const vec3& v2) {
    return vec3_normalize(vec3_cross(v0 - v1, v2 - v1));
}

vec3 vec3_rotate_x(const vec3& p, float rad);
vec3 vec3_rotate_y(const vec3& p, float rad);
vec3 vec3_rotate_yaw_pitch(const vec3& p, float yaw, float pitch);
float vec3_2d_angle(const vec3& a, const vec3& b);

#endif // VEC3_H