#include "entity_enemy.h"
#include "entity_projectile.h"
#include "entity_player.h"
#include "game.h"
#include "../renderer/renderer.h"
#include "../assets/map.h"
#include <cmath>
#include <algorithm>

// Stub declarations - will be loaded from assets
static model_t* model_grunt = nullptr;
static model_t* model_enforcer = nullptr;
static model_t* model_ogre = nullptr;
static model_t* model_zombie = nullptr;
static model_t* model_hound = nullptr;
static model_t* model_blood = nullptr;
static model_t* model_gib_pieces[4] = {nullptr, nullptr, nullptr, nullptr};

static void* sfx_enemy_hit = nullptr;
static void* sfx_enemy_gib = nullptr;
static void* sfx_shotgun_shoot = nullptr;
static void* sfx_nailgun_shoot = nullptr;
static void* sfx_grenade_shoot = nullptr;
static void* sfx_plasma_shoot = nullptr;
static void* sfx_zombie_hit = nullptr;
static void* sfx_hound_attack = nullptr;

// Map trace function stub
bool map_trace(const vec3& from, const vec3& to) {
    // TODO: Implement line of sight check
    return false;
}

// Base enemy implementation
entity_enemy_t::entity_enemy_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2) {
    
    // Initialize animations
    _ANIMS = {
        {1.0f, {0}},           // 0: Idle
        {0.40f, {1,2,3,4}},    // 1: Walk
        {0.20f, {1,2,3,4}},    // 2: Run
        {0.25f, {0,5,5,5}},    // 3: Attack prepare
        {0.25f, {5,0,0,0}}     // 4: Attack
    };
    
    // Initialize states
    _STATE_IDLE =           {0, 0, 0.1f, nullptr};
    _STATE_PATROL =         {1, 0.5f, 0.5f, nullptr};
    _STATE_FOLLOW =         {2, 1.0f, 0.3f, nullptr};
    _STATE_ATTACK_RECOVER = {0, 0, 0.1f, &_STATE_FOLLOW};
    _STATE_ATTACK_EXEC =    {4, 0, 0.4f, &_STATE_ATTACK_RECOVER};
    _STATE_ATTACK_PREPARE = {3, 0, 0.4f, &_STATE_ATTACK_EXEC};
    _STATE_ATTACK_AIM =     {0, 0, 0.1f, &_STATE_ATTACK_PREPARE};
    _STATE_EVADE =          {2, 1.0f, 0.8f, &_STATE_ATTACK_AIM};
}

void entity_enemy_t::_init(void* p1, void* p2) {
    int patrol_dir = p1 ? *static_cast<int*>(p1) : 0;
    
    s = vec3(12, 28, 12);
    _step_height = 17;
    _speed = 196;
    _target_yaw = _yaw;
    _state_update_at = 0;
    _attack_distance = 800;
    _evade_distance = 96;
    _attack_chance = 0.65f;
    _keep_off_ledges = true;
    _turn_bias = 1;
    
    _check_against = ENTITY_GROUP_PLAYER;
    
    game_entities_enemies.push_back(shared_from_this());
    
    // Set initial state based on patrol direction
    if (patrol_dir) {
        _set_state(&_STATE_PATROL);
        _target_yaw = (M_PI/2) * patrol_dir;
        _anim_time = static_cast<float>(rand()) / RAND_MAX;
    } else {
        _set_state(&_STATE_IDLE);
    }
}

void entity_enemy_t::_set_state(enemy_state_t* state) {
    _state = state;
    _anim = _ANIMS[state->anim_index];
    _anim_time = 0;
    _state_update_at = game_time + state->next_state_update + 
                      state->next_state_update/4 * (static_cast<float>(rand()) / RAND_MAX);
}

void entity_enemy_t::_update() {
    if (_state_update_at < game_time) {
        _turn_bias = (static_cast<float>(rand()) / RAND_MAX) > 0.5f ? 0.5f : -0.5f;
        
        if (!game_entity_player || game_entity_player->_dead) {
            return;
        }
        
        float distance_to_player = vec3_dist(p, game_entity_player->p);
        float angle_to_player = vec3_2d_angle(p, game_entity_player->p);
        
        if (_state->next_state) {
            _set_state(_state->next_state);
        }
        
        // State logic
        if (_state == &_STATE_FOLLOW) {
            if (!map_trace(p, game_entity_player->p)) {
                _target_yaw = angle_to_player;
            }
            
            if (distance_to_player < _attack_distance) {
                if (distance_to_player < _evade_distance || 
                    (static_cast<float>(rand()) / RAND_MAX) > _attack_chance) {
                    _set_state(&_STATE_EVADE);
                    _target_yaw += M_PI/2 + (static_cast<float>(rand()) / RAND_MAX) * M_PI;
                } else {
                    _set_state(&_STATE_ATTACK_AIM);
                }
            }
        }
        
        if (_state == &_STATE_ATTACK_RECOVER) {
            _target_yaw = angle_to_player;
        }
        
        if (_state == &_STATE_PATROL || _state == &_STATE_IDLE) {
            if (distance_to_player < 700 && !map_trace(p, game_entity_player->p)) {
                _set_state(&_STATE_ATTACK_AIM);
            }
        }
        
        if (_state == &_STATE_ATTACK_AIM) {
            _target_yaw = angle_to_player;
            if (map_trace(p, game_entity_player->p)) {
                _set_state(&_STATE_EVADE);
            }
        }
        
        if (_state == &_STATE_ATTACK_EXEC) {
            _attack();
        }
    }
    
    // Rotate to desired angle
    _yaw += anglemod(_target_yaw - _yaw) * 0.1f;
    
    // Move along yaw direction
    if (_on_ground) {
        v = vec3_rotate_y(vec3(0, v.y, _state->speed * _speed), _target_yaw);
    }
    
    _update_physics();
    _draw_model();
}

EntityPtr entity_enemy_t::_spawn_projectile(int type, float speed, float yaw_offset, float pitch_offset) {
    // TODO: Implement projectile spawning based on type
    return nullptr;
}

void entity_enemy_t::_receive_damage(EntityPtr from, float amount) {
    entity_t::_receive_damage(from, amount);
    _play_sound(sfx_enemy_hit);
    
    if (_state == &_STATE_IDLE || _state == &_STATE_PATROL) {
        _target_yaw = vec3_2d_angle(p, game_entity_player->p);
        _set_state(&_STATE_FOLLOW);
    }
    
    _spawn_particles(2, 200, model_blood, 18, 0.5f);
}

void entity_enemy_t::_kill() {
    entity_t::_kill();
    
    for (int i = 0; i < 4; i++) {
        if (model_gib_pieces[i]) {
            _spawn_particles(2, 300, model_gib_pieces[i], 18, 1.0f);
        }
    }
    
    _play_sound(sfx_enemy_gib);
    
    // Remove from enemy list
    game_entities_enemies.erase(
        std::remove(game_entities_enemies.begin(), game_entities_enemies.end(), shared_from_this()),
        game_entities_enemies.end()
    );
}

void entity_enemy_t::_did_collide(int axis) {
    if (axis == 1) return;
    
    if (_state == &_STATE_PATROL) {
        _target_yaw += M_PI;
    } else {
        _target_yaw += _turn_bias;
    }
}

// Grunt implementation
entity_enemy_grunt_t::entity_enemy_grunt_t(const vec3& pos, void* p1, void* p2)
    : entity_enemy_t(pos, p1, p2) {}

void entity_enemy_grunt_t::_init(void* p1, void* p2) {
    entity_enemy_t::_init(p1, p2);
    _health = 30;
    _model = model_grunt;
    _texture = 16;
}

void entity_enemy_grunt_t::_attack() {
    audio_play(sfx_shotgun_shoot);
    for (int i = 0; i < 8; i++) {
        float spread = 0.08f;
        _spawn_projectile(0, 10000, 
            (static_cast<float>(rand()) / RAND_MAX) * spread - spread/2,
            (static_cast<float>(rand()) / RAND_MAX) * spread - spread/2);
    }
}

// Enforcer implementation
entity_enemy_enforcer_t::entity_enemy_enforcer_t(const vec3& pos, void* p1, void* p2)
    : entity_enemy_t(pos, p1, p2) {}

void entity_enemy_enforcer_t::_init(void* p1, void* p2) {
    entity_enemy_t::_init(p1, p2);
    _health = 60;
    _speed = 128;
    _model = model_enforcer;
    _texture = 1;
}

void entity_enemy_enforcer_t::_attack() {
    audio_play(sfx_plasma_shoot);
    _spawn_projectile(1, 700, 0, 0);
}

// Ogre implementation
entity_enemy_ogre_t::entity_enemy_ogre_t(const vec3& pos, void* p1, void* p2)
    : entity_enemy_t(pos, p1, p2) {}

void entity_enemy_ogre_t::_init(void* p1, void* p2) {
    entity_enemy_t::_init(p1, p2);
    _health = 120;
    _model = model_ogre;
    _texture = 25;
}

void entity_enemy_ogre_t::_attack() {
    audio_play(sfx_grenade_shoot);
    _spawn_projectile(2, 600, 0, -0.4f);
}

// Zombie implementation
entity_enemy_zombie_t::entity_enemy_zombie_t(const vec3& pos, void* p1, void* p2)
    : entity_enemy_t(pos, p1, p2) {}

void entity_enemy_zombie_t::_init(void* p1, void* p2) {
    entity_enemy_t::_init(p1, p2);
    _health = 60;
    _speed = 64;
    _model = model_zombie;
    _texture = 20;
    _evade_distance = 32;
}

void entity_enemy_zombie_t::_attack() {
    audio_play(sfx_zombie_hit);
    if (game_entity_player) {
        game_entity_player->_receive_damage(shared_from_this(), 10);
    }
}

// Hound implementation
entity_enemy_hound_t::entity_enemy_hound_t(const vec3& pos, void* p1, void* p2)
    : entity_enemy_t(pos, p1, p2) {}

void entity_enemy_hound_t::_init(void* p1, void* p2) {
    entity_enemy_t::_init(p1, p2);
    _health = 25;
    _speed = 300;
    _model = model_hound;
    _texture = 26;
    _evade_distance = 24;
    s = vec3(20, 20, 20);
}

void entity_enemy_hound_t::_attack() {
    audio_play(sfx_hound_attack);
    if (game_entity_player) {
        game_entity_player->_receive_damage(shared_from_this(), 10);
    }
}