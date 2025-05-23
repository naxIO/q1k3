#include "input.h"
#include "../game/game.h"

Input* g_input = nullptr;

Input::Input() : mouse_x(0), mouse_y(0), last_wheel_event(0) {
    for (int i = 0; i < 16; i++) {
        keys[i] = false;
    }
    g_input = this;
}

void Input::handle_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (!event.key.repeat) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_UP:
                        keys[KEY_UP] = true;
                        break;
                    case SDLK_a:
                    case SDLK_LEFT:
                        keys[KEY_LEFT] = true;
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        keys[KEY_DOWN] = true;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        keys[KEY_RIGHT] = true;
                        break;
                    case SDLK_q:
                        keys[KEY_PREV] = true;
                        break;
                    case SDLK_e:
                        keys[KEY_NEXT] = true;
                        break;
                    case SDLK_SPACE:
                        keys[KEY_JUMP] = true;
                        break;
                }
            }
            break;
            
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    keys[KEY_UP] = false;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    keys[KEY_LEFT] = false;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    keys[KEY_DOWN] = false;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    keys[KEY_RIGHT] = false;
                    break;
                case SDLK_q:
                    keys[KEY_PREV] = false;
                    break;
                case SDLK_e:
                    keys[KEY_NEXT] = false;
                    break;
                case SDLK_SPACE:
                    keys[KEY_JUMP] = false;
                    break;
            }
            break;
            
        case SDL_MOUSEWHEEL:
            if (game_time - last_wheel_event > 0.1f) {
                if (event.wheel.y > 0) {
                    keys[KEY_PREV] = true;
                } else if (event.wheel.y < 0) {
                    keys[KEY_NEXT] = true;
                }
                last_wheel_event = game_time;
            }
            break;
            
        case SDL_MOUSEMOTION:
            mouse_x += event.motion.xrel;
            mouse_y += event.motion.yrel;
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                keys[KEY_ACTION] = true;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                keys[KEY_JUMP] = true;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                keys[KEY_ACTION] = false;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                keys[KEY_JUMP] = false;
            }
            break;
    }
}

void Input::reset_mouse_movement() {
    mouse_x = 0;
    mouse_y = 0;
    
    // Reset weapon switch keys after processing
    keys[KEY_PREV] = false;
    keys[KEY_NEXT] = false;
}