#ifndef ENTITY_ENEMY_H
#define ENTITY_ENEMY_H

#include "entity.h"
#include <array>

// Enemy state structure
struct enemy_state_t {
    int anim_index;
    float speed;
    float next_state_update;
    enemy_state_t* next_state;
};

// Base enemy class
class entity_enemy_t : public entity_t {
protected:
    // Animation definitions
    std::vector<std::pair<float, std::vector<int>>> _ANIMS;
    
    // States
    enemy_state_t _STATE_IDLE;
    enemy_state_t _STATE_PATROL;
    enemy_state_t _STATE_FOLLOW;
    enemy_state_t _STATE_ATTACK_RECOVER;
    enemy_state_t _STATE_ATTACK_EXEC;
    enemy_state_t _STATE_ATTACK_PREPARE;
    enemy_state_t _STATE_ATTACK_AIM;
    enemy_state_t _STATE_EVADE;
    
    float _speed;
    float _target_yaw;
    float _state_update_at;
    float _attack_distance;
    float _evade_distance;
    float _attack_chance;
    float _turn_bias;
    
    enemy_state_t* _state;
    
    void _set_state(enemy_state_t* state);
    EntityPtr _spawn_projectile(int type, float speed, float yaw_offset, float pitch_offset);
    
public:
    entity_enemy_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _receive_damage(EntityPtr from, float amount) override;
    void _kill() override;
    void _did_collide(int axis) override;
    
    virtual void _attack() = 0;  // Pure virtual - must be implemented by subclasses
};

// Grunt enemy
class entity_enemy_grunt_t : public entity_enemy_t {
public:
    entity_enemy_grunt_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _attack() override;
};

// Enforcer enemy  
class entity_enemy_enforcer_t : public entity_enemy_t {
public:
    entity_enemy_enforcer_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _attack() override;
};

// Ogre enemy
class entity_enemy_ogre_t : public entity_enemy_t {
public:
    entity_enemy_ogre_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _attack() override;
};

// Zombie enemy
class entity_enemy_zombie_t : public entity_enemy_t {
public:
    entity_enemy_zombie_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _attack() override;
};

// Hound enemy
class entity_enemy_hound_t : public entity_enemy_t {
public:
    entity_enemy_hound_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _attack() override;
};

#endif // ENTITY_ENEMY_H