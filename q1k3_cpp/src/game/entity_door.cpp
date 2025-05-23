#include "entity_door.h"
#include "entity_player.h"
#include "game.h"
#include "../renderer/renderer.h"
#include <algorithm>

// Model stub
static model_t* model_door = nullptr;

entity_door_t::entity_door_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2), _key(0), _open_at(0), _target(), _start() {
}

void entity_door_t::_init(void* p1, void* p2) {
    int texture = p1 ? *static_cast<int*>(p1) : 13;
    int dir = p2 ? *static_cast<int*>(p2) : 0;
    
    _model = model_door;
    _texture = texture;
    _health = 10;
    s = vec3(64, 64, 64);
    _start = vec3_clone(p);
    
    _open_at = 0;
    _yaw = dir * M_PI/2;
    _key = 0;  // 0 = closed, 1 = open
    
    // Map 1 only has one door and it needs a key
    bool needs_key = (game_map_index == 0);  // Note: JS uses 1-based, C++ uses 0-based
    
    // Doors block enemies and players
    game_entities_enemies.push_back(shared_from_this());
    game_entities_friendly.push_back(shared_from_this());
}

void entity_door_t::_update() {
    _draw_model();
    
    if (game_entity_player && vec3_dist(p, game_entity_player->p) < 128) {
        if (_key == 0) {  // If door needs key and is closed
            game_show_message("YOU NEED THE KEY...");
            return;
        }
        _open_at = game_time + 3;
    }
    
    float open_amount;
    if (_open_at < game_time) {
        open_amount = std::max(0.0f, static_cast<float>(_key) - game_tick);
    } else {
        open_amount = std::min(1.0f, static_cast<float>(_key) + game_tick);
    }
    _key = static_cast<int>(open_amount);
    
    p = _start + vec3_rotate_y(vec3(96 * open_amount, 0, 0), _yaw);
}

void entity_door_t::_did_collide_with_entity(EntityPtr other) {
    // Doors don't respond to entity collisions
}