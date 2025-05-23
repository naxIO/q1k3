#include "entity_particle.h"
#include "../renderer/renderer.h"

entity_particle_t::entity_particle_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2) {
}

void entity_particle_t::_init(void* /*p1*/, void* /*p2*/) {
    _gravity = 0.3f;
    _bounciness = 0.3f;
    _check_against = ENTITY_GROUP_NONE;
    f = 4;
}

void entity_particle_t::_update() {
    _update_physics();
    
    if (_model) {
        _draw_model();
    }
}