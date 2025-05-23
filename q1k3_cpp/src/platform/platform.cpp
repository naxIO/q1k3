#include "platform.h"
#include <GL/gl.h>
#include <iostream>

Platform* g_platform = nullptr;

Platform::Platform() : window(nullptr), gl_context(nullptr), running(false) {
    g_platform = this;
}

Platform::~Platform() {
    shutdown();
    g_platform = nullptr;
}

bool Platform::init(const std::string& title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create window
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create OpenGL context
    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    running = true;
    return true;
}

void Platform::shutdown() {
    if (gl_context) {
        SDL_GL_DeleteContext(gl_context);
        gl_context = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}

void Platform::swap_buffers() {
    SDL_GL_SwapWindow(window);
}

void Platform::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        // Input handling will be done in input.cpp
    }
}

float Platform::get_time() {
    return SDL_GetTicks() / 1000.0f;
}

void Platform::request_pointer_lock() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Platform::release_pointer_lock() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

bool Platform::is_pointer_locked() const {
    return SDL_GetRelativeMouseMode() == SDL_TRUE;
}

int Platform::get_width() const {
    int w;
    SDL_GetWindowSize(window, &w, nullptr);
    return w;
}

int Platform::get_height() const {
    int h;
    SDL_GetWindowSize(window, nullptr, &h);
    return h;
}