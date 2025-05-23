#include "ttt.h"
#include "renderer.h"
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <algorithm>

// Helper to convert 16-bit color to RGBA components
static void rgba_from_2byte(int color, GLubyte& r, GLubyte& g, GLubyte& b, GLubyte& a) {
    r = ((color >> 12) & 15) * 17;
    g = ((color >> 8) & 15) * 17;  
    b = ((color >> 4) & 15) * 17;
    a = (color & 15) * 17;
}

// Fill rectangle with color (including emboss effect)
static void fill_rect(GLubyte* data, int width, int height, 
                     int x, int y, int w, int h, 
                     int top_color, int bottom_color, int fill_color) {
    
    GLubyte tr, tg, tb, ta;
    GLubyte br, bg, bb, ba;
    GLubyte fr, fg, fb, fa;
    
    rgba_from_2byte(top_color, tr, tg, tb, ta);
    rgba_from_2byte(bottom_color, br, bg, bb, ba);
    rgba_from_2byte(fill_color, fr, fg, fb, fa);
    
    // Draw with emboss effect (top-left highlight, bottom-right shadow)
    for (int py = std::max(0, y); py < std::min(height, y + h); py++) {
        for (int px = std::max(0, x); px < std::min(width, x + w); px++) {
            int idx = (py * width + px) * 4;
            
            // Main fill
            if (px >= x && px < x + w && py >= y && py < y + h) {
                data[idx] = fr;
                data[idx + 1] = fg;
                data[idx + 2] = fb;
                data[idx + 3] = fa;
            }
            
            // Top/left highlight
            if ((px == x - 1 || py == y - 1) && px >= 0 && py >= 0) {
                data[idx] = tr;
                data[idx + 1] = tg;
                data[idx + 2] = tb;
                data[idx + 3] = ta;
            }
            
            // Bottom/right shadow
            if ((px == x + w || py == y + h) && px < width && py < height) {
                data[idx] = br;
                data[idx + 1] = bg;
                data[idx + 2] = bb;
                data[idx + 3] = ba;
            }
        }
    }
}

std::vector<ttt_texture_t> ttt_generate(const std::vector<std::vector<int>>& texture_data) {
    std::vector<ttt_texture_t> textures;
    
    for (const auto& d : texture_data) {
        int i = 0;
        int width = d[i++];
        int height = d[i++];
        
        ttt_texture_t tex;
        tex.width = width;
        tex.height = height;
        tex.data = new GLubyte[width * height * 4];
        
        // Fill with background color
        int bg_color = d[i++];
        fill_rect(tex.data, width, height, 0, 0, width, height, 0, 0, bg_color);
        
        // Process all operations for this texture
        while (i < d.size()) {
            int op = d[i++];
            
            switch (op) {
                case 0: { // Rectangle
                    int x = d[i++];
                    int y = d[i++];
                    int w = d[i++];
                    int h = d[i++];
                    int top = d[i++];
                    int bottom = d[i++];
                    int fill = d[i++];
                    fill_rect(tex.data, width, height, x, y, w, h, top, bottom, fill);
                    break;
                }
                
                case 1: { // Multiple rectangles
                    int sx = d[i++];
                    int sy = d[i++];
                    int w = d[i++];
                    int h = d[i++];
                    int inc_x = d[i++];
                    int inc_y = d[i++];
                    int top = d[i++];
                    int bottom = d[i++];
                    int fill = d[i++];
                    
                    for (int x = sx; x < width; x += inc_x) {
                        for (int y = sy; y < height; y += inc_y) {
                            fill_rect(tex.data, width, height, x, y, w, h, top, bottom, fill);
                        }
                    }
                    break;
                }
                
                case 2: { // Random noise
                    int color = d[i++];
                    int size = d[i++];
                    
                    for (int x = 0; x < width; x += size) {
                        for (int y = 0; y < height; y += size) {
                            int noise_color = (color & 0xfff0) + 
                                            static_cast<int>(static_cast<float>(rand()) / RAND_MAX * (color & 15));
                            fill_rect(tex.data, width, height, x, y, size, size, 0, 0, noise_color);
                        }
                    }
                    break;
                }
                
                case 3: { // Text (skip for now - would need font rendering)
                    i += 6; // Skip parameters
                    break;
                }
                
                case 4: { // Draw previous texture (skip for now - complex)
                    i += 6; // Skip parameters
                    break;
                }
            }
        }
        
        textures.push_back(tex);
    }
    
    return textures;
}

// Include texture data from JS
#include "textures.h"

void textures_init() {
    // Generate all textures
    auto generated_textures = ttt_generate(texture_data);
    
    // Create OpenGL textures
    for (const auto& tex : generated_textures) {
        r_create_texture(tex.data, tex.width, tex.height);
        delete[] tex.data;
    }
}