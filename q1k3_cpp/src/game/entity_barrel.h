#ifndef ENTITY_BARREL_H
#define ENTITY_BARREL_H

#include "entity.h"

class entity_barrel_t : public entity_t {
public:
    entity_barrel_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _kill() override;
    void _explode();
};

#endif // ENTITY_BARREL_H