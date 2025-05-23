#ifndef ENTITY_DOOR_H
#define ENTITY_DOOR_H

#include "entity.h"

class entity_door_t : public entity_t {
private:
    int _key;
    float _open_at;
    vec3 _target;
    vec3 _start;
    
public:
    entity_door_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _did_collide_with_entity(EntityPtr other) override;
};

#endif // ENTITY_DOOR_H