#ifndef MAP_H
#define MAP_H

#include <string>
#include "../core/vec3.h"

// Map functions
bool map_load_container(const std::string& path);
void map_init(int index);
void map_draw();
bool map_block_at(int x, int y, int z);
bool map_block_at_box(const vec3& min, const vec3& max);
bool map_trace(const vec3& from, const vec3& to);

#endif // MAP_H