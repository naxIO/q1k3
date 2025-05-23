#include "entity_light.h"
#include "../renderer/renderer.h"

entity_light_t::entity_light_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2), _intensity(1.0f), _color(1, 1, 1) {
}

void entity_light_t::_init(void* p1, void* p2) {
    // Parse parameters if provided
    if (p1) {
        _intensity = *static_cast<float*>(p1);
    }
    if (p2) {
        int color = *static_cast<int*>(p2);
        _color = vec3(
            ((color >> 16) & 0xFF) / 255.0f,
            ((color >> 8) & 0xFF) / 255.0f,
            (color & 0xFF) / 255.0f
        );
    }
}

void entity_light_t::_update() {
    // Add light to renderer
    r_push_light(p, _intensity, _color.x, _color.y, _color.z);
}