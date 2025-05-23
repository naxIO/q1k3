#include "map.h"
#include <iostream>

// Temporary stubs
bool map_load_container(const std::string& path) {
    std::cout << "Loading maps from: " << path << std::endl;
    return true;
}

void map_init(int index) {
    std::cout << "Initializing map " << index << std::endl;
    // TODO: Load map data, spawn entities
}

void map_draw() {
    // TODO: Draw map geometry
}

bool map_block_at(int x, int y, int z) {
    // TODO: Check collision map
    return false;
}

bool map_block_at_box(const vec3& min, const vec3& max) {
    // TODO: Check box collision against map
    return false;
}