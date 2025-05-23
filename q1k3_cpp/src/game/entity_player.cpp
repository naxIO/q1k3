#include "entity_player.h"
#include "../platform/input.h"
#include "../renderer/renderer.h"
#include "../core/math_utils.h"
#include "game.h"
#include <cmath>
#include <algorithm>

// Audio is included from weapons.h

// UI element stubs - will be replaced with proper UI system
void update_health_display(int health) {
    // TODO: Update UI
}

void update_ammo_display(const std::string& ammo) {
    // TODO: Update UI
}

entity_player_t::entity_player_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2), _speed(3000), _can_jump(false), _can_shoot_at(0),
      _weapon_index(0), _bob(0) {
}

void entity_player_t::_init(void* p1, void* p2) {
    s = vec3(12, 24, 12);
    f = 10;
    _step_height = 17;
    _health = 100;
    _check_against = ENTITY_GROUP_ENEMY;
    
    // Initialize weapons
    _weapons.push_back(std::make_unique<weapon_shotgun_t>());
    
    // Map 1 needs some rotation of the starting look-at direction
    _yaw += game_map_index * M_PI;
    
    game_entity_player = std::static_pointer_cast<entity_player_t>(shared_from_this());
    game_entities_friendly.push_back(shared_from_this());
}

void entity_player_t::_update() {
    // Mouse look
    float mouse_sensitivity = 0.00015f; // TODO: Get from settings
    _pitch = clamp(_pitch + g_input->get_mouse_y() * mouse_sensitivity, -1.5f, 1.5f);
    _yaw = std::fmod(_yaw + g_input->get_mouse_x() * mouse_sensitivity, M_PI * 2);
    
    // Acceleration in movement direction
    vec3 move_input(
        g_input->get_move_x(),
        0,
        -g_input->get_move_z()  // Negative because forward is -Z
    );
    
    a = vec3_rotate_y(move_input, _yaw) * (_speed * (_on_ground ? 1.0f : 0.3f));
    
    // Jump
    if (g_input->is_key_down(KEY_JUMP) && _on_ground && _can_jump) {
        v.y = 400;
        _on_ground = false;
        _can_jump = false;
    }
    if (!g_input->is_key_down(KEY_JUMP)) {
        _can_jump = true;
    }
    
    // Weapon switching
    int weapon_switch = g_input->get_weapon_switch();
    if (weapon_switch != 0) {
        _weapon_index = (_weapon_index + weapon_switch + _weapons.size()) % _weapons.size();
    }
    
    float shoot_wait = _can_shoot_at - game_time;
    weapon_t* weapon = _weapons[_weapon_index].get();
    
    // Shoot weapon
    if (g_input->is_key_down(KEY_ACTION) && shoot_wait < 0) {
        _can_shoot_at = game_time + weapon->_reload;
        
        if (weapon->_needs_ammo && weapon->_ammo == 0) {
            audio_play(sfx_no_ammo);
        } else {
            weapon->_shoot(p, _yaw, _pitch);
            // Spawn muzzle flash
            auto light = game_spawn<entity_light_t>(p);
            light->_die_at = game_time + 0.1f;
        }
    }
    
    // Update physics
    _bob += vec3_length(a) * 0.0001f;
    f = _on_ground ? 10.0f : 2.5f;
    _update_physics();
    
    // Update camera
    r_camera.x = p.x;
    r_camera.z = p.z;
    
    // Smooth step up on stairs
    r_camera.y = p.y + 8 - clamp(game_time - _stepped_up_at, 0.0f, 0.1f) * -160;
    
    r_camera_yaw = _yaw;
    r_camera_pitch = _pitch;
    
    // Draw weapon at camera position
    vec3 weapon_offset(
        0,
        -10 + std::sin(_bob) * 0.3f,
        12 + clamp(scale(shoot_wait, 0, weapon->_reload, 5, 0), 0.0f, 5.0f)
    );
    
    vec3 weapon_pos = r_camera + vec3_rotate_yaw_pitch(weapon_offset, _yaw, _pitch);
    
    r_draw(weapon_pos,
           _yaw + M_PI/2, _pitch,
           weapon->_texture, weapon->_model->f[0], weapon->_model->f[0], 0,
           weapon->_model->nv);
    
    // Update UI
    update_health_display(static_cast<int>(_health));
    update_ammo_display(weapon->_needs_ammo ? std::to_string(weapon->_ammo) : "∞");
}

void entity_player_t::_receive_damage(EntityPtr from, float amount) {
    audio_play(sfx_hurt);
    entity_t::_receive_damage(from, amount);
}

void entity_player_t::_kill() {
    entity_t::_kill();
    update_health_display(static_cast<int>(_health));
    title_show_message("YOU DIED");
    // TODO: Implement timer for respawn
    // setTimeout(() => game_init(game_map_index), 2000);
}