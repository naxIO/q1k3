#include "renderer.h"
#include "../core/math_utils.h"
#include "../platform/platform.h"
#include <iostream>
#include <cstring>
#include <algorithm>

// Global renderer variables
vec3 r_camera;
float r_camera_yaw = 0;
float r_camera_pitch = 0;

// Renderer state
static GLuint shader_program;
static GLuint vao, vbo;
static vertex_t* r_buffer;
int r_num_verts = 0;
static std::vector<light_t> r_lights;
static std::vector<draw_call_t> r_draw_calls;
static std::vector<texture_t> r_textures;
std::vector<model_t> r_models;

// Uniform locations
static GLint r_u_camera;
static GLint r_u_lights;
static GLint r_u_mouse;
static GLint r_u_pos;
static GLint r_u_rotation;
static GLint r_u_frame_mix;
static GLint r_u_texture;

// Vertex shader source (ported from JS)
const char* R_SOURCE_VS = R"(
#version 330 core

layout(location = 0) in vec3 p;    // position
layout(location = 1) in vec2 t;    // texture coord
layout(location = 2) in vec3 n;    // normal
layout(location = 3) in vec3 p2;   // mix position
layout(location = 4) in vec3 n2;   // mix normal

out vec3 vp, vn;
out vec2 vt;

uniform vec4 c;      // Camera position (xyz) and aspect ratio (w)
uniform vec3 mp;     // Model position
uniform vec2 mr;     // Model rotation (yaw, pitch)
uniform vec2 m;      // Mouse rotation (yaw, pitch)
uniform float f;     // Blend factor

mat4 rx(float r) {
    return mat4(
        1, 0, 0, 0,
        0, cos(r), sin(r), 0,
        0, -sin(r), cos(r), 0,
        0, 0, 0, 1
    );
}

mat4 ry(float r) {
    return mat4(
        cos(r), 0, -sin(r), 0,
        0, 1, 0, 0,
        sin(r), 0, cos(r), 0,
        0, 0, 0, 1
    );
}

mat4 rz(float r) {
    return mat4(
        cos(r), sin(r), 0, 0,
        -sin(r), cos(r), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

void main() {
    mat4 mry = ry(mr.x);
    mat4 mrz = rz(mr.y);
    
    vp = (mry * mrz * vec4(mix(p, p2, f), 1.0)).xyz + mp;
    vn = (mry * mrz * vec4(mix(n, n2, f), 1.0)).xyz;
    vt = t;
    
    mat4 projection = mat4(
        1, 0, 0, 0,
        0, c.w, 0, 0,
        0, 0, 1, 1,
        0, 0, -2, 0
    );
    
    gl_Position = projection * rx(-m.y) * ry(-m.x) * vec4(vp - c.xyz, 1.0);
}
)";

// Fragment shader source (ported from JS)
const char* R_SOURCE_FS = R"(
#version 330 core

in vec3 vp, vn;
in vec2 vt;

out vec4 FragColor;

uniform sampler2D s;
uniform vec3 l[64];  // Lights [(x,y,z), (r,g,b), ...]

void main() {
    FragColor = texture(s, vt);
    
    vec3 vl = vec3(0.0);
    for (int i = 0; i < 64; i += 2) {
        if (l[i] == vec3(0.0)) break;
        
        vec3 lightDir = normalize(l[i] - vp);
        float dist = length(l[i] - vp);
        float angle = max(dot(vn, lightDir), 0.0);
        float attenuation = 1.0 / (dist * dist);
        
        vl += angle * attenuation * l[i + 1];
    }
    
    // Gamma correction and color reduction
    FragColor.rgb = floor(FragColor.rgb * pow(vl, vec3(0.75)) * 16.0 + 0.5) / 16.0;
}
)";

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // Check compilation
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compilation failed: " << info << std::endl;
    }
    
    return shader;
}

bool r_init() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // Allocate vertex buffer
    r_buffer = new vertex_t[R_MAX_VERTS];
    
    // Create and compile shaders
    GLuint vs = compile_shader(GL_VERTEX_SHADER, R_SOURCE_VS);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, R_SOURCE_FS);
    
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);
    
    // Check linking
    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(shader_program, 512, nullptr, info);
        std::cerr << "Shader linking failed: " << info << std::endl;
        return false;
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    glUseProgram(shader_program);
    
    // Get uniform locations
    r_u_camera = glGetUniformLocation(shader_program, "c");
    r_u_lights = glGetUniformLocation(shader_program, "l");
    r_u_mouse = glGetUniformLocation(shader_program, "m");
    r_u_pos = glGetUniformLocation(shader_program, "mp");
    r_u_rotation = glGetUniformLocation(shader_program, "mr");
    r_u_frame_mix = glGetUniformLocation(shader_program, "f");
    r_u_texture = glGetUniformLocation(shader_program, "s");
    
    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, R_MAX_VERTS * sizeof(vertex_t), nullptr, GL_DYNAMIC_DRAW);
    
    // Setup vertex attributes
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);
    
    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, u));
    glEnableVertexAttribArray(1);
    
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
    glEnableVertexAttribArray(2);
    
    // Mix position (p2) - will be offset later for animated models
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, pos));
    glEnableVertexAttribArray(3);
    
    // Mix normal (n2) - will be offset later for animated models
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
    glEnableVertexAttribArray(4);
    
    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set viewport
    glViewport(0, 0, g_platform->get_width(), g_platform->get_height());
    
    return true;
}

void r_cleanup() {
    delete[] r_buffer;
    glDeleteProgram(shader_program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    
    for (auto& tex : r_textures) {
        glDeleteTextures(1, &tex.id);
    }
}

void r_prepare_frame(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    r_lights.clear();
    r_draw_calls.clear();
}

void r_end_frame() {
    // Upload light data
    float light_buffer[R_MAX_LIGHTS * 6];
    std::memset(light_buffer, 0, sizeof(light_buffer));
    
    for (size_t i = 0; i < r_lights.size() && i < R_MAX_LIGHTS; i++) {
        light_buffer[i * 6 + 0] = r_lights[i].pos.x;
        light_buffer[i * 6 + 1] = r_lights[i].pos.y;
        light_buffer[i * 6 + 2] = r_lights[i].pos.z;
        light_buffer[i * 6 + 3] = r_lights[i].color.x;
        light_buffer[i * 6 + 4] = r_lights[i].color.y;
        light_buffer[i * 6 + 5] = r_lights[i].color.z;
    }
    
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    
    // Set uniforms
    float aspect = static_cast<float>(g_platform->get_width()) / g_platform->get_height();
    glUniform4f(r_u_camera, r_camera.x, r_camera.y, r_camera.z, aspect);
    glUniform2f(r_u_mouse, r_camera_yaw, r_camera_pitch);
    glUniform3fv(r_u_lights, R_MAX_LIGHTS * 2, light_buffer);
    
    // Sort draw calls by texture
    std::sort(r_draw_calls.begin(), r_draw_calls.end(),
        [](const draw_call_t& a, const draw_call_t& b) { return a.texture < b.texture; });
    
    // Draw all calls
    int last_texture = -1;
    int vertex_offset = 0;
    
    for (const auto& call : r_draw_calls) {
        // Bind texture if changed
        if (last_texture != call.texture) {
            last_texture = call.texture;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, r_textures[call.texture].id);
            glUniform1i(r_u_texture, 0);
        }
        
        // Set model uniforms
        glUniform3f(r_u_pos, call.pos.x, call.pos.y, call.pos.z);
        glUniform2f(r_u_rotation, call.yaw, call.pitch);
        glUniform1f(r_u_frame_mix, call.mix);
        
        // Update vertex attribute pointers for animation if needed
        if (vertex_offset != (call.offset2 - call.offset1)) {
            vertex_offset = call.offset2 - call.offset1;
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
                (void*)(vertex_offset * sizeof(vertex_t) + offsetof(vertex_t, pos)));
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
                (void*)(vertex_offset * sizeof(vertex_t) + offsetof(vertex_t, normal)));
        }
        
        glDrawArrays(GL_TRIANGLES, call.offset1, call.num_verts);
    }
}

void r_draw(const vec3& pos, float yaw, float pitch, int texture,
            int frame1, int frame2, float mix, int num_verts) {
    r_draw_calls.push_back({pos, yaw, pitch, texture, frame1, frame2, mix, num_verts});
}

void r_push_light(const vec3& pos, float intensity, float r, float g, float b) {
    // Calculate fade based on distance
    float dist = vec3_dist(pos, r_camera);
    float fade = clamp(scale(dist, 768, 1024, 1, 0), 0, 1) * intensity * 10;
    
    if (fade > 0 && r_lights.size() < R_MAX_LIGHTS) {
        r_lights.push_back({pos, vec3(r * fade, g * fade, b * fade)});
    }
}

void r_submit_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, r_num_verts * sizeof(vertex_t), r_buffer);
}

int r_push_vert(const vec3& pos, const vec3& normal, float u, float v) {
    if (r_num_verts >= R_MAX_VERTS) return r_num_verts;
    
    r_buffer[r_num_verts] = {pos, u, v, normal};
    return r_num_verts++;
}

void r_push_quad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, float u, float v) {
    vec3 n = vec3_face_normal(v0, v1, v2);
    r_push_vert(v0, n, u, 0);
    r_push_vert(v1, n, 0, 0);
    r_push_vert(v2, n, u, v);
    r_push_vert(v3, n, 0, v);
    r_push_vert(v2, n, u, v);
    r_push_vert(v1, n, 0, 0);
}

int r_push_block(float x, float y, float z, float sx, float sy, float sz, int texture) {
    int index = r_num_verts;
    
    float tx = sx / r_textures[texture].width;
    float ty = sy / r_textures[texture].height;
    float tz = sz / r_textures[texture].width;
    
    // Define vertices
    vec3 v0(x, y + sy, z);
    vec3 v1(x + sx, y + sy, z);
    vec3 v2(x, y + sy, z + sz);
    vec3 v3(x + sx, y + sy, z + sz);
    vec3 v4(x, y, z + sz);
    vec3 v5(x + sx, y, z + sz);
    vec3 v6(x, y, z);
    vec3 v7(x + sx, y, z);
    
    // Push all faces
    r_push_quad(v0, v1, v2, v3, tx, tz); // top
    r_push_quad(v4, v5, v6, v7, tx, tz); // bottom
    r_push_quad(v2, v3, v4, v5, tx, ty); // front
    r_push_quad(v1, v0, v7, v6, tx, ty); // back
    r_push_quad(v3, v1, v5, v7, tz, ty); // right
    r_push_quad(v0, v2, v6, v4, tz, ty); // left
    
    return index;
}

void r_create_texture(GLubyte* data, int width, int height) {
    texture_t tex;
    tex.width = width;
    tex.height = height;
    
    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    r_textures.push_back(tex);
}

// model_load_container is defined in model.cpp

model_t* model_get(int index) {
    if (index >= 0 && index < r_models.size()) {
        return &r_models[index];
    }
    return nullptr;
}