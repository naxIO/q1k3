#include "entity_projectile.h"
#include "entity_light.h"
#include "../renderer/renderer.h"

// Stub for model_explosion - will be loaded from assets
static model_t* model_explosion = nullptr;
static model_t* model_nail = nullptr;
static model_t* model_grenade = nullptr;
static model_t* model_plasma = nullptr;
static model_t* model_gib = nullptr;

// Base projectile
entity_projectile_t::entity_projectile_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2) {}

// Shell projectile
entity_projectile_shell_t::entity_projectile_shell_t(const vec3& pos, void* p1, void* p2)
    : entity_projectile_t(pos, p1, p2) {}

void entity_projectile_shell_t::_init(void* /*p1*/, void* /*p2*/) {
    _gravity = 0;
    _die_at = game_time + 0.1f;
}

void entity_projectile_shell_t::_update() {
    _update_physics();
}

void entity_projectile_shell_t::_did_collide(int /*axis*/) {
    _kill();
    _spawn_particles(2, 80, model_explosion, 4, 0.4f);
    auto light = game_spawn<entity_light_t>(p);
    float intensity = 0.5f;
    int color = 0xff;
    light->_init(&intensity, &color);
    light->_die_at = game_time + 0.1f;
}

void entity_projectile_shell_t::_did_collide_with_entity(EntityPtr other) {
    _kill();
    other->_receive_damage(shared_from_this(), 4);
}

// Nail projectile
entity_projectile_nail_t::entity_projectile_nail_t(const vec3& pos, void* p1, void* p2)
    : entity_projectile_t(pos, p1, p2) {}

void entity_projectile_nail_t::_init(void* /*p1*/, void* /*p2*/) {
    _gravity = 0;
    _model = model_nail;
    _texture = 8;
    _die_at = game_time + 3;
}

void entity_projectile_nail_t::_update() {
    _update_physics();
    _draw_model();
}

void entity_projectile_nail_t::_did_collide(int /*axis*/) {
    _kill();
    _spawn_particles(3, 200, model_explosion, 4, 0.3f);
}

void entity_projectile_nail_t::_did_collide_with_entity(EntityPtr other) {
    _kill();
    other->_receive_damage(shared_from_this(), 15);
}

// Grenade projectile
entity_projectile_grenade_t::entity_projectile_grenade_t(const vec3& pos, void* p1, void* p2)
    : entity_projectile_t(pos, p1, p2), _explode_at(0) {}

void entity_projectile_grenade_t::_init(void* /*p1*/, void* /*p2*/) {
    _gravity = 1;
    _bounciness = 0.5f;
    f = 5;
    _model = model_grenade;
    _texture = 21;
    _die_at = game_time + 4;
    _explode_at = game_time + 1.5f;
}

void entity_projectile_grenade_t::_update() {
    _update_physics();
    _draw_model();
    
    if (_explode_at && game_time > _explode_at) {
        _explode();
    }
}

void entity_projectile_grenade_t::_did_collide(int axis) {
    if (axis == 1 && v.y < -100) {
        _explode();
    }
}

void entity_projectile_grenade_t::_explode() {
    _spawn_particles(32, 300, model_explosion, 4, 0.5f);
    
    // Damage nearby enemies
    for (auto& entity : game_entities) {
        float dist = vec3_dist(entity->p, p);
        if (dist < 128 && entity.get() != this) {
            entity->_receive_damage(shared_from_this(), (128 - dist) / 5);
        }
    }
    
    auto light = game_spawn<entity_light_t>(p);
    float intensity = 100.0f;
    int color = 0xffa020;
    light->_init(&intensity, &color);
    light->_die_at = game_time + 0.5f;
    
    _kill();
}

// Plasma projectile
entity_projectile_plasma_t::entity_projectile_plasma_t(const vec3& pos, void* p1, void* p2)
    : entity_projectile_t(pos, p1, p2) {}

void entity_projectile_plasma_t::_init(void* /*p1*/, void* /*p2*/) {
    _gravity = 0;
    s = vec3(4, 4, 4);
    _model = model_plasma;
    _texture = 29;
    _anim = {0.1f, {0, 1, 2, 3}};
    _die_at = game_time + 2;
}

void entity_projectile_plasma_t::_update() {
    _update_physics();
    _draw_model();
    r_push_light(p, 0.5f, 1, 0.7f, 0.7f);
}

void entity_projectile_plasma_t::_did_collide(int /*axis*/) {
    _kill();
    _spawn_particles(3, 200, model_explosion, 22, 0.3f);
}

void entity_projectile_plasma_t::_did_collide_with_entity(EntityPtr other) {
    _kill();
    other->_receive_damage(shared_from_this(), 20);
}

// Gib projectile
entity_projectile_gib_t::entity_projectile_gib_t(const vec3& pos, void* p1, void* p2)
    : entity_projectile_t(pos, p1, p2) {}

void entity_projectile_gib_t::_init(void* /*p1*/, void* /*p2*/) {
    f = 12;
    _bounciness = 0.5f;
    _model = model_gib;
    _texture = 11;
    _die_at = game_time + 5;
}

void entity_projectile_gib_t::_update() {
    _update_physics();
    _draw_model();
}

void entity_projectile_gib_t::_did_collide(int /*axis*/) {
    v = v * 0.8f;
}