#ifndef WEAPONS_H
#define WEAPONS_H

#include "../core/vec3.h"
#include <memory>

// Forward declarations
class entity_t;
class entity_projectile_shell_t;
class entity_projectile_nail_t;
class entity_projectile_grenade_t;
class entity_light_t;
struct model_t;
void audio_play(void* sound);

// Sound effect declarations
extern void* sfx_shotgun_shoot;
extern void* sfx_shotgun_reload;
extern void* sfx_nailgun_shoot;
extern void* sfx_grenade_shoot;
extern void* sfx_no_ammo;

// Model declarations
extern model_t* model_shotgun;
extern model_t* model_nailgun;
extern model_t* model_grenadelauncher;

class weapon_t {
public:
    bool _needs_ammo;
    vec3 _projectile_offset;
    int _texture;
    model_t* _model;
    void* _sound;
    float _reload;
    int _ammo;
    float _projectile_speed;
    
    weapon_t();
    virtual ~weapon_t() = default;
    
    virtual void _init() = 0;
    virtual void _shoot(const vec3& pos, float yaw, float pitch);
    virtual void _spawn_projectile(const vec3& pos, float yaw, float pitch);
};

class weapon_shotgun_t : public weapon_t {
public:
    void _init() override;
    void _spawn_projectile(const vec3& pos, float yaw, float pitch) override;
};

class weapon_nailgun_t : public weapon_t {
public:
    void _init() override;
};

class weapon_grenadelauncher_t : public weapon_t {
public:
    void _init() override;
};

#endif // WEAPONS_H