#ifndef ENTITY_PLAYER_H
#define ENTITY_PLAYER_H

#include "entity.h"
#include "weapons.h"
#include <vector>
#include <memory>

class entity_player_t : public entity_t {
private:
    float _speed;
    bool _can_jump;
    float _can_shoot_at;
    std::vector<std::unique_ptr<weapon_t>> _weapons;
    int _weapon_index;
    float _bob;
    
public:
    entity_player_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _receive_damage(EntityPtr from, float amount) override;
    void _kill() override;
};

#endif // ENTITY_PLAYER_H