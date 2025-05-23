#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

// Key indices matching the JavaScript version
enum GameKey {
    KEY_UP = 1,
    KEY_LEFT = 2,
    KEY_DOWN = 3,
    KEY_RIGHT = 4,
    KEY_PREV = 5,
    KEY_NEXT = 6,
    KEY_ACTION = 7,
    KEY_JUMP = 9
};

class Input {
private:
    bool keys[16];  // Array to hold key states
    float mouse_x, mouse_y;
    float last_wheel_event;
    
public:
    Input();
    
    void handle_event(const SDL_Event& event);
    void reset_mouse_movement();
    
    bool is_key_down(GameKey key) const { return keys[key]; }
    float get_mouse_x() const { return mouse_x; }
    float get_mouse_y() const { return mouse_y; }
    
    // Helper functions for movement
    float get_move_x() const { return keys[KEY_RIGHT] - keys[KEY_LEFT]; }
    float get_move_z() const { return keys[KEY_UP] - keys[KEY_DOWN]; }
    int get_weapon_switch() const { return keys[KEY_NEXT] - keys[KEY_PREV]; }
};

extern Input* g_input;

#endif // INPUT_H