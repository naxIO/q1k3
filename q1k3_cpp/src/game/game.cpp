#include "game.h"
#include "entity.h"
#include "../platform/platform.h"
#include "../platform/input.h"
#include "../renderer/renderer.h"
#include "../assets/map.h"
#include <algorithm>
#include <iostream>

// Global game variables
float game_tick = 0;
float game_time = 0.016f;
float game_real_time_last = 0;
int game_message_timeout = 0;

std::vector<EntityPtr> game_entities;
std::vector<EntityPtr> game_entities_enemies;
std::vector<EntityPtr> game_entities_friendly;
std::shared_ptr<entity_player_t> game_entity_player;
int game_map_index = 0;
bool game_jump_to_next_level = false;

void game_init(int map_index) {
    // Clear entity lists
    game_entities.clear();
    game_entities_enemies.clear();
    game_entities_friendly.clear();
    
    game_map_index = map_index;
    
    // Initialize map
    map_init(map_index);
}

void game_next_level() {
    game_jump_to_next_level = true;
}

void game_show_message(const std::string& text) {
    // TODO: Implement UI message display
    std::cout << "Game message: " << text << std::endl;
    game_message_timeout = 2000; // 2 seconds
}

void title_show_message(const std::string& msg, const std::string& sub) {
    // TODO: Implement title screen display
    std::cout << msg << std::endl;
    if (!sub.empty()) {
        std::cout << sub << std::endl;
    }
}

void game_update() {
    // Update all entities
    std::vector<EntityPtr> alive_entities;
    
    for (auto& entity : game_entities) {
        if (!entity->_dead) {
            entity->_update_physics();
            entity->_update();
            alive_entities.push_back(entity);
        }
    }
    
    game_entities = alive_entities;
    
    // Handle level transition
    if (game_jump_to_next_level) {
        game_jump_to_next_level = false;
        game_map_index++;
        
        if (game_map_index == 2) {
            title_show_message("THE END", "THANKS FOR PLAYING ❤");
            if (game_entity_player) {
                game_entity_player->_dead = true;
            }
            
            // Set camera position for end screen
            r_camera = vec3(1856, 784, 2272);
            r_camera_yaw = 0;
            r_camera_pitch = 0.5f;
        } else {
            game_init(game_map_index);
        }
    }
}

void game_run(float time_now) {
    // Calculate delta time
    game_tick = std::min(time_now - game_real_time_last, 0.05f);
    game_real_time_last = time_now;
    game_time += game_tick;
    
    // Clear frame
    r_prepare_frame(0.1f, 0.2f, 0.5f);
    
    // Update game logic
    game_update();
    
    // Draw map
    map_draw();
    
    // Finish rendering
    r_end_frame();
    
    // Reset input for next frame
    g_input->reset_mouse_movement();
}

void game_cleanup() {
    game_entities.clear();
    game_entities_enemies.clear();
    game_entities_friendly.clear();
    game_entity_player.reset();
}