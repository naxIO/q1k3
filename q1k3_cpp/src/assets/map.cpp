#include "map.h"
#include "../game/entity.h"
#include "../game/entity_player.h"
#include "../game/entity_enemy.h"
#include "../game/entity_pickup.h"
#include "../game/entity_door.h"
#include "../game/entity_barrel.h"
#include "../game/entity_torch.h"
#include "../game/entity_trigger_level.h"
#include "../renderer/renderer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

// Map size constant
static const int MAP_SIZE = 128;

// Map data structures
struct block_t {
    uint8_t x, y, z;
    uint8_t sx, sy, sz;
};

struct entity_data_t {
    char type;
    uint8_t x, y, z;
    uint8_t data1, data2;
};

struct map_t {
    uint8_t* collision_map;  // Bitmap for collision
    std::vector<entity_data_t> entities;
    std::vector<std::pair<int, int>> render_blocks; // texture, vertex_offset
};

// Global map data
static std::vector<map_t> maps;
static map_t* current_map = nullptr;

bool map_load_container(const std::string& path) {
    // Load all maps from container file
    std::ifstream file(path + "l", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open map container: " << path << std::endl;
        return false;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read entire file
    std::vector<uint8_t> data(file_size);
    file.read(reinterpret_cast<char*>(data.data()), file_size);
    file.close();
    
    // Parse maps
    size_t i = 0;
    while (i < data.size()) {
        map_t map;
        
        // Read blocks size
        uint16_t blocks_size = data[i] | (data[i + 1] << 8);
        i += 2;
        
        // Allocate collision map (bitmap)
        size_t cm_size = (MAP_SIZE * MAP_SIZE * MAP_SIZE) >> 3;
        map.collision_map = new uint8_t[cm_size];
        std::memset(map.collision_map, 0, cm_size);
        
        // Parse blocks
        size_t blocks_end = i + blocks_size;
        int current_texture = 0;
        
        while (i < blocks_end) {
            // Check for texture sentinel
            if (data[i] == 255) {
                i++;
                current_texture = data[i++];
                continue;
            }
            
            // Read block data
            block_t block;
            block.x = data[i++];
            block.y = data[i++];
            block.z = data[i++];
            block.sx = data[i++];
            block.sy = data[i++];
            block.sz = data[i++];
            
            // Add block to renderer
            int vertex_offset = r_push_block(
                block.x << 5, block.y << 4, block.z << 5,
                block.sx << 5, block.sy << 4, block.sz << 5,
                current_texture
            );
            
            map.render_blocks.push_back({current_texture, vertex_offset});
            
            // Update collision map
            for (int cz = block.z; cz < block.z + block.sz; cz++) {
                for (int cy = block.y; cy < block.y + block.sy; cy++) {
                    for (int cx = block.x; cx < block.x + block.sx; cx++) {
                        int bit_index = cz * MAP_SIZE * MAP_SIZE + cy * MAP_SIZE + cx;
                        map.collision_map[bit_index >> 3] |= 1 << (cx & 7);
                    }
                }
            }
        }
        
        // Read entities
        uint16_t num_entities = data[i] | (data[i + 1] << 8);
        i += 2;
        
        for (int e = 0; e < num_entities; e++) {
            entity_data_t entity;
            entity.type = data[i++];
            entity.x = data[i++];
            entity.y = data[i++];
            entity.z = data[i++];
            entity.data1 = data[i++];
            entity.data2 = data[i++];
            map.entities.push_back(entity);
        }
        
        maps.push_back(map);
    }
    
    // Submit vertex buffer to renderer
    r_submit_buffer();
    
    std::cout << "Loaded " << maps.size() << " maps" << std::endl;
    return true;
}

void map_init(int index) {
    if (index >= maps.size()) {
        std::cerr << "Invalid map index: " << index << std::endl;
        return;
    }
    
    current_map = &maps[index];
    
    // Entity spawn table - must match map_packer.c
    typedef EntityPtr (*spawn_func_t)(const vec3&, void*, void*);
    
    // Define spawn functions
    static spawn_func_t spawn_player = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_player_t>(p, d1, d2); };
    static spawn_func_t spawn_grunt = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_enemy_grunt_t>(p, d1, d2); };
    static spawn_func_t spawn_enforcer = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_enemy_enforcer_t>(p, d1, d2); };
    static spawn_func_t spawn_ogre = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_enemy_ogre_t>(p, d1, d2); };
    static spawn_func_t spawn_zombie = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_enemy_zombie_t>(p, d1, d2); };
    static spawn_func_t spawn_hound = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_enemy_hound_t>(p, d1, d2); };
    static spawn_func_t spawn_health = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_pickup_health_t>(p, d1, d2); };
    static spawn_func_t spawn_nailgun = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_pickup_nailgun_t>(p, d1, d2); };
    static spawn_func_t spawn_grenadelauncher = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_pickup_grenadelauncher_t>(p, d1, d2); };
    static spawn_func_t spawn_nails = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_pickup_nails_t>(p, d1, d2); };
    static spawn_func_t spawn_grenades = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_pickup_grenades_t>(p, d1, d2); };
    static spawn_func_t spawn_key = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_pickup_key_t>(p, d1, d2); };
    static spawn_func_t spawn_door = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_door_t>(p, d1, d2); };
    static spawn_func_t spawn_barrel = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_barrel_t>(p, d1, d2); };
    static spawn_func_t spawn_torch = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_torch_t>(p, d1, d2); };
    static spawn_func_t spawn_trigger_level = [](const vec3& p, void* d1, void* d2) -> EntityPtr { return game_spawn<entity_trigger_level_t>(p, d1, d2); };
    
    spawn_func_t spawn_table[] = {
        /* 00 */ spawn_player,
        /* 01 */ spawn_grunt,
        /* 02 */ spawn_enforcer,
        /* 03 */ spawn_ogre,
        /* 04 */ spawn_zombie,
        /* 05 */ spawn_hound,
        /* 06 */ spawn_health,
        /* 07 */ spawn_nailgun,
        /* 08 */ spawn_grenadelauncher,
        /* 09 */ spawn_nails,
        /* 10 */ spawn_grenades,
        /* 11 */ spawn_key,
        /* 12 */ spawn_door,
        /* 13 */ spawn_barrel,
        /* 14 */ spawn_torch,
        /* 15 */ spawn_trigger_level,
    };
    
    // Spawn all entities
    for (const auto& entity : current_map->entities) {
        if (entity.type < sizeof(spawn_table) / sizeof(spawn_table[0])) {
            vec3 pos(entity.x << 5, entity.y << 4, entity.z << 5);
            spawn_table[entity.type](pos, const_cast<uint8_t*>(&entity.data1), const_cast<uint8_t*>(&entity.data2));
        }
    }
}

void map_draw() {
    if (!current_map) return;
    
    // Draw all blocks
    for (const auto& block : current_map->render_blocks) {
        // The renderer will handle batching by texture
        r_draw(vec3(), 0, 0, block.first, block.second, block.second, 0, 36); // 36 verts per block
    }
}

bool map_block_at(int x, int y, int z) {
    if (!current_map || x < 0 || y < 0 || z < 0 || 
        x >= MAP_SIZE || y >= MAP_SIZE || z >= MAP_SIZE) {
        return true; // Out of bounds = solid
    }
    
    int bit_index = z * MAP_SIZE * MAP_SIZE + y * MAP_SIZE + x;
    return (current_map->collision_map[bit_index >> 3] & (1 << (x & 7))) != 0;
}

bool map_block_at_box(const vec3& min, const vec3& max) {
    // Check all blocks that the box intersects
    int x0 = static_cast<int>(min.x) >> 5;
    int y0 = static_cast<int>(min.y) >> 4;
    int z0 = static_cast<int>(min.z) >> 5;
    int x1 = static_cast<int>(max.x) >> 5;
    int y1 = static_cast<int>(max.y) >> 4;
    int z1 = static_cast<int>(max.z) >> 5;
    
    for (int z = z0; z <= z1; z++) {
        for (int y = y0; y <= y1; y++) {
            for (int x = x0; x <= x1; x++) {
                if (map_block_at(x, y, z)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool map_trace(const vec3& from, const vec3& to) {
    // Simple line trace for line of sight
    vec3 dir = to - from;
    float len = vec3_length(dir);
    dir = vec3_normalize(dir);
    
    for (float t = 0; t < len; t += 16) {
        vec3 pos = from + dir * t;
        if (map_block_at(pos.x / 32, pos.y / 16, pos.z / 32)) {
            return true;
        }
    }
    
    return false;
}