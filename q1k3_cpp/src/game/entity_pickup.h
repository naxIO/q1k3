#ifndef ENTITY_PICKUP_H
#define ENTITY_PICKUP_H

#include "entity.h"

// Base pickup class
class entity_pickup_t : public entity_t {
protected:
    vec3 _bob_offset;
    float _bob_time;
    
public:
    entity_pickup_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    
    void _update() override;
    void _did_collide_with_entity(EntityPtr other) override;
    virtual void _pickup(EntityPtr other) = 0;
};

// Health pickup
class entity_pickup_health_t : public entity_pickup_t {
public:
    entity_pickup_health_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _pickup(EntityPtr other) override;
};

// Nailgun pickup
class entity_pickup_nailgun_t : public entity_pickup_t {
public:
    entity_pickup_nailgun_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _pickup(EntityPtr other) override;
};

// Grenadelauncher pickup
class entity_pickup_grenadelauncher_t : public entity_pickup_t {
public:
    entity_pickup_grenadelauncher_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _pickup(EntityPtr other) override;
};

// Nails ammo pickup
class entity_pickup_nails_t : public entity_pickup_t {
public:
    entity_pickup_nails_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _pickup(EntityPtr other) override;
};

// Grenades ammo pickup
class entity_pickup_grenades_t : public entity_pickup_t {
public:
    entity_pickup_grenades_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _pickup(EntityPtr other) override;
};

// Key pickup
class entity_pickup_key_t : public entity_pickup_t {
public:
    entity_pickup_key_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    void _init(void* p1, void* p2) override;
    void _pickup(EntityPtr other) override;
};

#endif // ENTITY_PICKUP_H