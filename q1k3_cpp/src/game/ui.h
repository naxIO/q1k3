#ifndef UI_H
#define UI_H

#include <string>
#include <GL/glew.h>

class UI {
private:
    // Text rendering with bitmap font
    static void render_text(float x, float y, const std::string& text, float scale = 1.0f);
    
    // UI state
    static bool title_screen_visible;
    static std::string title_message;
    static std::string title_submessage;
    
    static bool game_message_visible;
    static std::string game_message;
    static float game_message_timeout;
    
    static int player_health;
    static std::string player_ammo;
    
public:
    static void init();
    static void cleanup();
    
    // Update UI
    static void update(float delta_time);
    
    // Render UI
    static void render();
    
    // UI functions matching JS
    static void show_title_screen(const std::string& message, const std::string& submessage = "");
    static void hide_title_screen();
    
    static void show_game_message(const std::string& message);
    
    static void update_health(int health);
    static void update_ammo(const std::string& ammo);
};

#endif // UI_H