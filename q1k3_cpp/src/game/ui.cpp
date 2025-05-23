#include "ui.h"
#include "../platform/platform.h"
#include "../renderer/renderer.h"
#include <GL/glew.h>

// Static member definitions
bool UI::title_screen_visible = true;
std::string UI::title_message = "Q1K3";
std::string UI::title_submessage = "CLICK TO START";
bool UI::game_message_visible = false;
std::string UI::game_message;
float UI::game_message_timeout = 0;
int UI::player_health = 100;
std::string UI::player_ammo = "∞";

// Simple bitmap font
static GLuint font_texture = 0;
static const int CHAR_WIDTH = 8;
static const int CHAR_HEIGHT = 16;
static const int CHARS_PER_ROW = 16;

// Generate a simple bitmap font texture
static void generate_font_texture() {
    const int tex_width = 128;
    const int tex_height = 128;
    GLubyte* font_data = new GLubyte[tex_width * tex_height * 4];
    
    // Clear to transparent
    std::memset(font_data, 0, tex_width * tex_height * 4);
    
    // Generate simple block characters for ASCII 32-127
    for (int ch = 32; ch < 128; ch++) {
        int row = (ch - 32) / CHARS_PER_ROW;
        int col = (ch - 32) % CHARS_PER_ROW;
        
        int x_offset = col * CHAR_WIDTH;
        int y_offset = row * CHAR_HEIGHT;
        
        // Create a simple filled rectangle for each character
        // (In a real implementation, we'd use actual font data)
        for (int y = 2; y < CHAR_HEIGHT - 2; y++) {
            for (int x = 2; x < CHAR_WIDTH - 2; x++) {
                int idx = ((y_offset + y) * tex_width + (x_offset + x)) * 4;
                font_data[idx] = 255;     // R
                font_data[idx + 1] = 255; // G
                font_data[idx + 2] = 255; // B
                font_data[idx + 3] = 255; // A
            }
        }
    }
    
    // Create OpenGL texture
    glGenTextures(1, &font_texture);
    glBindTexture(GL_TEXTURE_2D, font_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, font_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    delete[] font_data;
}

void UI::init() {
    generate_font_texture();
}

void UI::cleanup() {
    if (font_texture) {
        glDeleteTextures(1, &font_texture);
        font_texture = 0;
    }
}

void UI::update(float delta_time) {
    // Update message timeout
    if (game_message_visible && game_message_timeout > 0) {
        game_message_timeout -= delta_time;
        if (game_message_timeout <= 0) {
            game_message_visible = false;
        }
    }
}

void UI::render() {
    // Save OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    int width = g_platform->get_width();
    int height = g_platform->get_height();
    glOrtho(0, width, height, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable depth test for UI
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Render title screen
    if (title_screen_visible) {
        // Title text (centered, large)
        render_text(width / 2, height / 2 - 50, title_message, 4.0f);
        
        // Subtitle (centered, smaller)
        if (!title_submessage.empty()) {
            render_text(width / 2, height / 2 + 50, title_submessage, 1.5f);
        }
    }
    
    // Render game UI
    if (!title_screen_visible) {
        // Health (bottom left)
        render_text(width * 0.2f, height * 0.9f, std::to_string(player_health), 2.0f);
        
        // Ammo (bottom right)
        render_text(width * 0.8f, height * 0.9f, player_ammo, 2.0f);
        
        // Game message (top center)
        if (game_message_visible) {
            render_text(width / 2, height * 0.1f, game_message, 1.2f);
        }
    }
    
    // Restore OpenGL state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}

void UI::render_text(float x, float y, const std::string& text, float scale) {
    if (font_texture == 0) return;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font_texture);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    float char_width = CHAR_WIDTH * scale;
    float char_height = CHAR_HEIGHT * scale;
    float start_x = x - (text.length() * char_width) / 2;
    
    glBegin(GL_QUADS);
    for (size_t i = 0; i < text.length(); i++) {
        char ch = text[i];
        if (ch >= 32 && ch < 128) {
            // Calculate texture coordinates
            int char_idx = ch - 32;
            int row = char_idx / CHARS_PER_ROW;
            int col = char_idx % CHARS_PER_ROW;
            
            float u0 = col * CHAR_WIDTH / 128.0f;
            float v0 = row * CHAR_HEIGHT / 128.0f;
            float u1 = (col + 1) * CHAR_WIDTH / 128.0f;
            float v1 = (row + 1) * CHAR_HEIGHT / 128.0f;
            
            float cx = start_x + i * char_width;
            
            // Draw character quad
            glTexCoord2f(u0, v0);
            glVertex2f(cx, y);
            
            glTexCoord2f(u1, v0);
            glVertex2f(cx + char_width, y);
            
            glTexCoord2f(u1, v1);
            glVertex2f(cx + char_width, y + char_height);
            
            glTexCoord2f(u0, v1);
            glVertex2f(cx, y + char_height);
        }
    }
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

void UI::show_title_screen(const std::string& message, const std::string& submessage) {
    title_screen_visible = true;
    title_message = message;
    title_submessage = submessage;
}

void UI::hide_title_screen() {
    title_screen_visible = false;
}

void UI::show_game_message(const std::string& message) {
    game_message = message;
    game_message_visible = true;
    game_message_timeout = 2.0f; // 2 seconds
}

void UI::update_health(int health) {
    player_health = health;
}

void UI::update_ammo(const std::string& ammo) {
    player_ammo = ammo;
}