#ifndef RENDERER_H
#define RENDERER_H

#include "../core/vec3.h"
#include <vector>
#include <string>
#include <GL/glew.h>

// Constants
const int R_MAX_VERTS = 1024 * 64;
const int R_MAX_LIGHTS = 32;

// Global renderer variables
extern vec3 r_camera;
extern float r_camera_yaw;
extern float r_camera_pitch;

// Vertex structure (matches JS: x,y,z, u,v, nx,ny,nz)
struct vertex_t {
    vec3 pos;
    float u, v;
    vec3 normal;
};

// Draw call structure
struct draw_call_t {
    vec3 pos;
    float yaw, pitch;
    int texture;
    int offset1, offset2;
    float mix;
    int num_verts;
};

// Light structure
struct light_t {
    vec3 pos;
    vec3 color;  // color * intensity
};

// Model structure
struct model_t {
    std::vector<int> f;  // Frame offsets in vertex buffer
    int nv;              // Number of vertices per frame
};

// Texture structure
struct texture_t {
    GLuint id;
    int width, height;
};

// Renderer functions
bool r_init();
void r_cleanup();
void r_prepare_frame(float r, float g, float b);
void r_end_frame();
void r_draw(const vec3& pos, float yaw, float pitch, int texture,
            int frame1, int frame2, float mix, int num_verts);
void r_push_light(const vec3& pos, float intensity, float r, float g, float b);
void r_submit_buffer();

// Geometry building
int r_push_vert(const vec3& pos, const vec3& normal, float u, float v);
void r_push_quad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, float u, float v);
int r_push_block(float x, float y, float z, float sx, float sy, float sz, int texture);

// Texture functions
void r_create_texture(GLubyte* data, int width, int height);

// Model loading
bool model_load_container(const std::string& path);
model_t* model_get(int index);

#endif // RENDERER_H