#ifndef TTT_H
#define TTT_H

#include <vector>
#include <GL/glew.h>

// TTT (Tiny Texture Tumbler) - Procedural texture generation
// Ported from JavaScript to C++

struct ttt_texture_t {
    GLubyte* data;
    int width;
    int height;
};

// Generate all textures from texture data
std::vector<ttt_texture_t> ttt_generate(const std::vector<std::vector<int>>& texture_data);

// Initialize texture system with procedural textures
void textures_init();

#endif // TTT_H