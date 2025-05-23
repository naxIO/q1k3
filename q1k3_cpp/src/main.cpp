#include <iostream>
#include "platform/platform.h"
#include "platform/input.h"
#include "game/game.h"
#include "renderer/renderer.h"
#include "assets/map.h"

int main(int argc, char* argv[]) {
    // Initialize platform
    Platform platform;
    if (!platform.init("Q1K3 C++", 1280, 720)) {
        std::cerr << "Failed to initialize platform!" << std::endl;
        return -1;
    }
    
    // Initialize input
    Input input;
    
    // Initialize renderer
    if (!r_init()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    // Load assets
    if (!map_load_container("assets/maps/")) {
        std::cerr << "Failed to load maps!" << std::endl;
        return -1;
    }
    
    if (!model_load_container("assets/models/")) {
        std::cerr << "Failed to load models!" << std::endl;
        return -1;
    }
    
    // Initialize game
    game_init(0);
    
    // Request pointer lock
    platform.request_pointer_lock();
    
    // Main game loop
    while (platform.is_running()) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                platform.quit();
            } else {
                input.handle_event(event);
            }
        }
        
        // Update and render game
        game_run(platform.get_time());
        
        // Swap buffers
        platform.swap_buffers();
    }
    
    // Cleanup
    game_cleanup();
    r_cleanup();
    
    return 0;
}