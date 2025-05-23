#ifndef ENTITY_TRIGGER_LEVEL_H
#define ENTITY_TRIGGER_LEVEL_H

#include "entity.h"

class entity_trigger_level_t : public entity_t {
public:
    entity_trigger_level_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _did_collide_with_entity(EntityPtr other) override;
};

#endif // ENTITY_TRIGGER_LEVEL_H