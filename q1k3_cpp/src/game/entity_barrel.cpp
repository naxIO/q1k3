#include "entity_barrel.h"
#include "entity_light.h"
#include "game.h"
#include "../renderer/renderer.h"
#include "../core/math_utils.h"
#include <algorithm>

// Model and sound stubs
static model_t* model_barrel = nullptr;
static model_t* model_gib_pieces[4] = {nullptr, nullptr, nullptr, nullptr};
static void* sfx_grenade_explode = nullptr;

entity_barrel_t::entity_barrel_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2) {
}

void entity_barrel_t::_init(void* /*p1*/, void* /*p2*/) {
    _model = model_barrel;
    _texture = 21;
    _pitch = M_PI/2;
    _health = 10;
    s = vec3(8, 32, 8);
    
    game_entities_enemies.push_back(shared_from_this());
}

void entity_barrel_t::_update() {
    _draw_model();
}

void entity_barrel_t::_kill() {
    _explode();
    entity_t::_kill();
}

void entity_barrel_t::_explode() {
    // Deal damage to nearby entities
    for (auto& entity : game_entities_enemies) {
        float dist = vec3_dist(p, entity->p);
        if (entity.get() != this && dist < 256) {
            entity->_receive_damage(shared_from_this(), scale(dist, 0, 256, 60, 0));
        }
    }
    
    _play_sound(sfx_grenade_explode);
    
    // Spawn gib particles
    for (int i = 0; i < 4; i++) {
        if (model_gib_pieces[i]) {
            _spawn_particles(2, 600, model_gib_pieces[i], 21, 1.0f);
        }
    }
    
    // Spawn explosion light
    auto light = game_spawn<entity_light_t>(p + vec3(0, 16, 0));
    float intensity = 250.0f;
    int color = 0x0088ff;
    light->_init(&intensity, &color);
    light->_die_at = game_time + 0.2f;
    
    // Remove from enemy list
    game_entities_enemies.erase(
        std::remove(game_entities_enemies.begin(), game_entities_enemies.end(), shared_from_this()),
        game_entities_enemies.end()
    );
}