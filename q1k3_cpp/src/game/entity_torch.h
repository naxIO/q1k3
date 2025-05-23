#ifndef ENTITY_TORCH_H
#define ENTITY_TORCH_H

#include "entity.h"

class entity_torch_t : public entity_t {
public:
    entity_torch_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
};

#endif // ENTITY_TORCH_H