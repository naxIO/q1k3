#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <memory>
#include "../core/vec3.h"
#include "../core/math_utils.h"

enum EntityGroup {
    ENTITY_GROUP_NONE = 0,
    ENTITY_GROUP_PLAYER = 1,
    ENTITY_GROUP_ENEMY = 2
};

class entity_t;
using EntityPtr = std::shared_ptr<entity_t>;

// Forward declarations
struct model_t;
class entity_particle_t;
class entity_light_t;
void r_draw(const vec3& pos, float yaw, float pitch, int texture, 
            int frame1, int frame2, float mix, int num_verts);
extern vec3 r_camera;
extern float r_camera_yaw;
void audio_play(void* sound, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f);
bool map_block_at(int x, int y, int z);
bool map_block_at_box(const vec3& min, const vec3& max);

// Global entity lists (defined in game.cpp)
extern std::vector<EntityPtr> game_entities;
extern std::vector<EntityPtr> game_entities_friendly;
extern std::vector<EntityPtr> game_entities_enemies;
extern float game_time;
extern float game_tick;

class entity_t : public std::enable_shared_from_this<entity_t> {
public:
    vec3 a;  // acceleration
    vec3 v;  // velocity  
    vec3 p;  // position
    vec3 s;  // size
    float f; // friction

    float _health;
    bool _dead;
    float _die_at;
    float _step_height;
    float _bounciness;
    float _gravity;
    float _yaw;
    float _pitch;
    std::pair<float, std::vector<int>> _anim;
    float _anim_time;
    bool _on_ground;
    bool _keep_off_ledges;
    
    EntityGroup _check_against;
    float _stepped_up_at;
    
    // Model and texture
    model_t* _model;
    int _texture;
    
    // Entities to check collisions against
    std::vector<EntityPtr> _check_entities;

    entity_t(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr);
    virtual ~entity_t() = default;
    
    virtual void _init(void* /*p1*/, void* /*p2*/) {}
    virtual void _update();
    virtual void _update_physics();
    virtual void _did_collide(int /*axis*/) {}
    virtual void _did_collide_with_entity(EntityPtr /*other*/) {}
    
    void _draw_model();
    bool _collides(const vec3& p);
    void _spawn_particles(int amount, float speed, model_t* model, int texture, float lifetime);
    virtual void _receive_damage(EntityPtr from, float amount);
    void _play_sound(void* sound);
    virtual void _kill();
};

// Template spawn function (implementation here to avoid linker issues)
template<typename T>
std::shared_ptr<T> game_spawn(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr) {
    auto entity = std::make_shared<T>(pos, p1, p2);
    entity->_init(p1, p2);  // Call _init after construction
    game_entities.push_back(entity);
    return entity;
}

#endif // ENTITY_H