#include "entity_torch.h"
#include "../renderer/renderer.h"
#include "../assets/map.h"
#include <cmath>
#include <cstdlib>

// Model stub
static model_t* model_torch = nullptr;

// Map trace function declaration
extern bool map_trace(const vec3& from, const vec3& to);

entity_torch_t::entity_torch_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2) {
}

void entity_torch_t::_init(void* /*p1*/, void* /*p2*/) {
    _texture = 30;
    _model = model_torch;
    
    _anim = {0.05f, {0,1,2,1,2,0,0,1,2}};
    
    p.x -= 16;
    p.z -= 16;
    vec3 light_pos = p;
    
    // Find which wall we're on
    vec3 trace_dirs[4] = {
        vec3(-32, 0, 0),
        vec3(32, 0, 0),
        vec3(0, 0, -32),
        vec3(0, 0, 32)
    };
    
    for (const vec3& trace_dir : trace_dirs) {
        vec3 trace_end = p + trace_dir;
        if (map_trace(p, trace_end)) {
            p = p + trace_dir * 0.4f;
            light_pos = p - trace_dir * 2.0f;
            break;
        }
    }
    
    // Store light position in entity (would need to add member variable)
    // For now, just use offset from position
}

void entity_torch_t::_update() {
    entity_t::_update();
    
    float light_flicker = 0;
    if (static_cast<float>(rand()) / RAND_MAX > 0.8f) {
        light_flicker = static_cast<float>(rand()) / RAND_MAX;
    }
    
    // Calculate light position (offset from torch position)
    vec3 light_pos = p + vec3(0, 0, 0);  // Would need proper offset based on wall
    
    r_push_light(light_pos, 
                 std::sin(game_time) + light_flicker + 6, 
                 1.0f, 0.75f, 0.0625f);  // RGB: 255,192,16 normalized
}