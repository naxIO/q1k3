#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <functional>
#include <string>

class Platform {
private:
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool running;
    
public:
    Platform();
    ~Platform();
    
    bool init(const std::string& title, int width, int height);
    void shutdown();
    
    bool is_running() const { return running; }
    void quit() { running = false; }
    
    void swap_buffers();
    void poll_events();
    
    // Time management
    float get_time();
    
    // Mouse control
    void request_pointer_lock();
    void release_pointer_lock();
    bool is_pointer_locked() const;
    
    // Window info
    int get_width() const;
    int get_height() const;
};

extern Platform* g_platform;

#endif // PLATFORM_H