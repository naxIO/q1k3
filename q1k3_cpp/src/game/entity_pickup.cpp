#include "entity_pickup.h"
#include "entity_player.h"
#include "entity_light.h"
#include "weapons.h"
#include "game.h"
#include "../renderer/renderer.h"
#include <cmath>

// Model stubs
static model_t* model_pickup_box = nullptr;
static model_t* model_pickup_key = nullptr;

// Sound stubs  
static void* sfx_pickup = nullptr;
static void* sfx_no_ammo_pickup = nullptr;

// Base pickup implementation
entity_pickup_t::entity_pickup_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2), _bob_offset(), _bob_time(0) {
}

void entity_pickup_t::_update() {
    if (!_on_ground) {
        _update_physics();
    }
    _draw_model();
    
    if (game_entity_player && vec3_dist(p, game_entity_player->p) < 40) {
        _pickup(game_entity_player);
    }
}

void entity_pickup_t::_did_collide_with_entity(EntityPtr other) {
    // Pickups don't collide with entities
}

// Health pickup
entity_pickup_health_t::entity_pickup_health_t(const vec3& pos, void* p1, void* p2)
    : entity_pickup_t(pos, p1, p2) {}

void entity_pickup_health_t::_init(void* /*p1*/, void* /*p2*/) {
    _model = model_pickup_box;
    s = vec3(12, 12, 12);
    _yaw += M_PI/2;
    _texture = 9;
}

void entity_pickup_health_t::_pickup(EntityPtr other) {
    other->_health = std::min(other->_health + 25.0f, 100.0f);
    game_show_message("HEALTH");
    audio_play(sfx_pickup);
    
    // Spawn light effect
    auto light = game_spawn<entity_light_t>(p);
    float intensity = 0.5f;
    int color = 0x00ff00;
    light->_init(&intensity, &color);
    light->_die_at = game_time + 0.1f;
    
    _kill();
}

// Nailgun pickup
entity_pickup_nailgun_t::entity_pickup_nailgun_t(const vec3& pos, void* p1, void* p2)
    : entity_pickup_t(pos, p1, p2) {}

void entity_pickup_nailgun_t::_init(void* /*p1*/, void* /*p2*/) {
    _model = model_nailgun;
    s = vec3(20, 8, 20);
    _texture = 4;
}

void entity_pickup_nailgun_t::_pickup(EntityPtr other) {
    // Assuming player has weapon system - would need proper casting
    game_show_message("NAILGUN");
    audio_play(sfx_pickup);
    
    auto light = game_spawn<entity_light_t>(p);
    float intensity = 0.5f;
    int color = 0x0000ff;
    light->_init(&intensity, &color);
    light->_die_at = game_time + 0.1f;
    
    _kill();
}

// Grenade launcher pickup
entity_pickup_grenadelauncher_t::entity_pickup_grenadelauncher_t(const vec3& pos, void* p1, void* p2)
    : entity_pickup_t(pos, p1, p2) {}

void entity_pickup_grenadelauncher_t::_init(void* /*p1*/, void* /*p2*/) {
    _model = model_grenadelauncher;
    s = vec3(20, 8, 20);
    _texture = 21;
}

void entity_pickup_grenadelauncher_t::_pickup(EntityPtr other) {
    game_show_message("GRENADE LAUNCHER");
    audio_play(sfx_pickup);
    
    auto light = game_spawn<entity_light_t>(p);
    float intensity = 0.5f;
    int color = 0xff0000;
    light->_init(&intensity, &color);
    light->_die_at = game_time + 0.1f;
    
    _kill();
}

// Nails ammo pickup
entity_pickup_nails_t::entity_pickup_nails_t(const vec3& pos, void* p1, void* p2)
    : entity_pickup_t(pos, p1, p2) {}

void entity_pickup_nails_t::_init(void* /*p1*/, void* /*p2*/) {
    _model = model_pickup_box;
    s = vec3(12, 12, 12);
    _yaw += M_PI/2;
    _texture = 5;
}

void entity_pickup_nails_t::_pickup(EntityPtr other) {
    // Would need to check if player has nailgun and add ammo
    game_show_message("NAILS");
    audio_play(sfx_pickup);
    _kill();
}

// Grenades ammo pickup
entity_pickup_grenades_t::entity_pickup_grenades_t(const vec3& pos, void* p1, void* p2)
    : entity_pickup_t(pos, p1, p2) {}

void entity_pickup_grenades_t::_init(void* /*p1*/, void* /*p2*/) {
    _model = model_pickup_box;
    s = vec3(12, 12, 12);
    _yaw += M_PI/2;
    _texture = 22;
}

void entity_pickup_grenades_t::_pickup(EntityPtr other) {
    // Would need to check if player has grenade launcher and add ammo
    game_show_message("GRENADES");
    audio_play(sfx_pickup);
    _kill();
}

// Key pickup
entity_pickup_key_t::entity_pickup_key_t(const vec3& pos, void* p1, void* p2)
    : entity_pickup_t(pos, p1, p2) {}

void entity_pickup_key_t::_init(void* /*p1*/, void* /*p2*/) {
    _model = model_pickup_key;
    s = vec3(12, 12, 12);
    _texture = 12;
}

void entity_pickup_key_t::_pickup(EntityPtr other) {
    // Would need to add key to player inventory
    game_show_message("KEY");
    audio_play(sfx_pickup);
    
    auto light = game_spawn<entity_light_t>(p);
    float intensity = 0.5f;
    int color = 0xffff00;
    light->_init(&intensity, &color);
    light->_die_at = game_time + 0.1f;
    
    _kill();
}