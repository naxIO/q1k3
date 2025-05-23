#ifndef ENTITY_PARTICLE_H
#define ENTITY_PARTICLE_H

#include "entity.h"

class entity_particle_t : public entity_t {
public:
    entity_particle_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
};

#endif // ENTITY_PARTICLE_H