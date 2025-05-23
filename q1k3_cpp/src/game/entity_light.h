#ifndef ENTITY_LIGHT_H
#define ENTITY_LIGHT_H

#include "entity.h"

class entity_light_t : public entity_t {
private:
    float _intensity;
    vec3 _color;
    
public:
    entity_light_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
};

#endif // ENTITY_LIGHT_H