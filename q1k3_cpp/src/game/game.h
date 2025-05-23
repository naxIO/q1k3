#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include <string>
#include "../core/vec3.h"

// Forward declarations
class entity_t;
class entity_player_t;
using EntityPtr = std::shared_ptr<entity_t>;

// Global game variables
extern float game_tick;
extern float game_time;
extern float game_real_time_last;
extern int game_message_timeout;

extern std::vector<EntityPtr> game_entities;
extern std::vector<EntityPtr> game_entities_enemies;
extern std::vector<EntityPtr> game_entities_friendly;
extern std::shared_ptr<entity_player_t> game_entity_player;
extern int game_map_index;
extern bool game_jump_to_next_level;

// Game functions
void game_init(int map_index);
void game_next_level();
void game_show_message(const std::string& text);
void title_show_message(const std::string& msg, const std::string& sub = "");
void game_run(float time_now);
void game_update();
void game_cleanup();

// Template spawn function
template<typename T>
std::shared_ptr<T> game_spawn(const vec3& pos, void* p1 = nullptr, void* p2 = nullptr) {
    auto entity = std::make_shared<T>(pos, p1, p2);
    game_entities.push_back(entity);
    
    // Add to specific lists based on type
    // This will be expanded as we add entity types
    
    return entity;
}

#endif // GAME_H