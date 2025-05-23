#include <iostream>
#include "platform/platform.h"
#include "platform/input.h"
#include "game/game.h"
#include "game/audio.h"
#include "game/ui.h"
#include "renderer/renderer.h"
#include "renderer/ttt.h"
#include "assets/map.h"

int main(int /*argc*/, char* /*argv*/[]) {
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
    
    // Generate textures
    textures_init();
    
    // Initialize audio
    if (!audio_init()) {
        std::cerr << "Failed to initialize audio!" << std::endl;
        return -1;
    }
    
    // Initialize UI
    UI::init();
    
    // Load assets
    if (!map_load_container("assets/")) {
        std::cerr << "Failed to load maps!" << std::endl;
        return -1;
    }
    
    if (!model_load_container("assets/")) {
        std::cerr << "Failed to load models!" << std::endl;
        return -1;
    }
    
    // Initialize game
    game_init(0);
    
    // Show title screen
    UI::show_title_screen("Q1K3", "CLICK TO START");
    
    // Main game loop
    bool game_started = false;
    while (platform.is_running()) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                platform.quit();
            } else if (event.type == SDL_MOUSEBUTTONDOWN && !game_started) {
                // Start game on first click
                game_started = true;
                UI::hide_title_screen();
                platform.request_pointer_lock();
                audio_play_music();
            } else {
                input.handle_event(event);
            }
        }
        
        // Update and render game
        float time = platform.get_time();
        game_run(time);
        
        // Update and render UI
        UI::update(game_tick);
        UI::render();
        
        // Swap buffers
        platform.swap_buffers();
    }
    
    // Cleanup
    game_cleanup();
    UI::cleanup();
    audio_cleanup();
    r_cleanup();
    
    return 0;
}