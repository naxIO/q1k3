#include "weapons.h"
#include "entity.h"
#include "timer.h"
#include "../platform/platform.h"
#include "../renderer/renderer.h"
#include <cstdlib>
#include <functional>

// Sound effects are defined in audio.cpp

// Model definitions
model_t* model_shotgun = nullptr;
model_t* model_nailgun = nullptr;
model_t* model_grenadelauncher = nullptr;

weapon_t::weapon_t() : _needs_ammo(true), _projectile_offset(0, 0, 8) {
    // _init() will be called by derived classes
}

void weapon_t::_shoot(const vec3& pos, float yaw, float pitch) {
    if (_needs_ammo) {
        _ammo--;
    }
    audio_play(_sound);
    _spawn_projectile(pos, yaw, pitch);
}

void weapon_t::_spawn_projectile(const vec3& pos, float yaw, float pitch) {
    // Calculate spawn position
    vec3 spawn_pos = pos + vec3(0, 12, 0) + vec3_rotate_yaw_pitch(_projectile_offset, yaw, pitch);
    
    // Spawn projectile based on type
    // This will be implemented when we have the projectile classes
    
    // Alternate left/right fire for next projectile (nailgun)
    _projectile_offset.x *= -1;
}

weapon_shotgun_t::weapon_shotgun_t() {
    _init();
}

void weapon_shotgun_t::_init() {
    _texture = 7;
    _model = model_shotgun;
    _sound = sfx_shotgun_shoot;
    _needs_ammo = false;
    _reload = 0.9f;
    _projectile_speed = 10000;
}

void weapon_shotgun_t::_spawn_projectile(const vec3& pos, float yaw, float pitch) {
    setTimeout([]() { audio_play(sfx_shotgun_reload); }, 200);
    setTimeout([]() { audio_play(sfx_shotgun_reload); }, 350);
    
    // Spawn 8 pellets with spread
    for (int i = 0; i < 8; i++) {
        float spread_yaw = yaw + (static_cast<float>(rand()) / RAND_MAX) * 0.08f - 0.04f;
        float spread_pitch = pitch + (static_cast<float>(rand()) / RAND_MAX) * 0.08f - 0.04f;
        weapon_t::_spawn_projectile(pos, spread_yaw, spread_pitch);
    }
}

weapon_nailgun_t::weapon_nailgun_t() {
    _init();
}

void weapon_nailgun_t::_init() {
    _texture = 4;
    _model = model_nailgun;
    _sound = sfx_nailgun_shoot;
    _ammo = 100;
    _reload = 0.09f;
    _projectile_speed = 1300;
    _projectile_offset = vec3(6, 0, 8);
}

weapon_grenadelauncher_t::weapon_grenadelauncher_t() {
    _init();
}

void weapon_grenadelauncher_t::_init() {
    _texture = 21;
    _model = model_grenadelauncher;
    _sound = sfx_grenade_shoot;
    _ammo = 10;
    _reload = 0.650f;
    _projectile_speed = 900;
}