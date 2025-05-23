#ifndef ENTITY_PROJECTILE_H
#define ENTITY_PROJECTILE_H

#include "entity.h"

// Base projectile class
class entity_projectile_t : public entity_t {
public:
    entity_projectile_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
};

// Shell projectile (shotgun)
class entity_projectile_shell_t : public entity_projectile_t {
public:
    entity_projectile_shell_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _did_collide(int axis) override;
    void _did_collide_with_entity(EntityPtr other) override;
};

// Nail projectile (nailgun)
class entity_projectile_nail_t : public entity_projectile_t {
public:
    entity_projectile_nail_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _did_collide(int axis) override;
    void _did_collide_with_entity(EntityPtr other) override;
};

// Grenade projectile
class entity_projectile_grenade_t : public entity_projectile_t {
private:
    float _explode_at;
    
public:
    entity_projectile_grenade_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _did_collide(int axis) override;
    void _explode();
};

// Plasma projectile (enemy)
class entity_projectile_plasma_t : public entity_projectile_t {
public:
    entity_projectile_plasma_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _did_collide(int axis) override;
    void _did_collide_with_entity(EntityPtr other) override;
};

// Gib projectile (gore)
class entity_projectile_gib_t : public entity_projectile_t {
public:
    entity_projectile_gib_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _init(void* p1, void* p2) override;
    void _update() override;
    void _did_collide(int axis) override;
};

#endif // ENTITY_PROJECTILE_H