#include "renderer.h"
#include <fstream>
#include <iostream>
#include <cstring>

// Global model storage
extern std::vector<model_t> r_models;
extern int r_num_verts;

// Forward declaration
static void model_init(uint8_t* data, size_t size, float sx = 1, float sy = 1, float sz = 1);

bool model_load_container(const std::string& path) {
    // Load all models from container file
    std::ifstream file(path + "m", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open model container: " << path << std::endl;
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
    
    // Parse RMF (Retarded Model Format)
    size_t i = 0;
    while (i < data.size()) {
        // Read header
        uint8_t num_frames = data[i++];
        uint8_t num_verts = data[i++];
        uint8_t num_indices = data[i++];
        
        // Calculate model size and create model data
        size_t model_size = (num_frames * num_verts + num_indices) * 3;
        
        // Parse this model
        model_init(&data[i - 3], model_size + 3, 1, 1, 1);
        
        i += model_size;
    }
    
    std::cout << "Loaded " << r_models.size() << " models" << std::endl;
    return true;
}

static void model_init(uint8_t* data, size_t size, float sx, float sy, float sz) {
    model_t model;
    
    // Parse header
    int j = 0;
    uint8_t num_frames = data[j++];
    uint8_t num_vertices = data[j++];
    uint8_t num_indices = data[j++];
    
    model.nv = num_indices * 3; // 3 verts per triangle
    
    // Load vertices
    std::vector<float> vertices(num_vertices * num_frames * 3);
    std::vector<uint8_t> indices(num_indices * 3);
    
    float min_x = 16, max_x = -16;
    float min_y = 16, max_y = -16;
    
    // Load and transform vertices
    for (int i = 0; i < num_vertices * num_frames * 3; i += 3) {
        vertices[i] = (data[j++] - 15) * sx;
        vertices[i + 1] = (data[j++] - 15) * sy;
        vertices[i + 2] = (data[j++] - 15) * sz;
        
        // Find bounds for first frame (for UV generation)
        if (i < num_vertices * 3) {
            min_x = std::min(min_x, vertices[i]);
            max_x = std::max(max_x, vertices[i]);
            min_y = std::min(min_y, vertices[i + 1]);
            max_y = std::max(max_y, vertices[i + 1]);
        }
    }
    
    // Load indices
    uint8_t index_increment = 0;
    for (int i = 0; i < num_indices * 3; i += 3) {
        index_increment += data[j++];
        indices[i] = index_increment;
        indices[i + 1] = data[j++];
        indices[i + 2] = data[j++];
    }
    
    // UV coordinate factors
    float uf = 1.0f / (max_x - min_x);
    float u = -min_x * uf;
    float vf = -1.0f / (max_y - min_y);
    float v = max_y * vf;
    
    // Process each frame
    for (int frame = 0; frame < num_frames; frame++) {
        model.f.push_back(r_num_verts);
        
        int vertex_offset = frame * num_vertices * 3;
        
        // Process each triangle
        for (int i = 0; i < num_indices * 3; i += 3) {
            vec3 verts[3];
            float uvs[3][2];
            
            // Get triangle vertices
            for (int j = 0; j < 3; j++) {
                int idx = indices[i + j] * 3;
                verts[j] = vec3(
                    vertices[vertex_offset + idx],
                    vertices[vertex_offset + idx + 1],
                    vertices[vertex_offset + idx + 2]
                );
                
                // Generate UVs based on model space position
                uvs[j][0] = vertices[idx] * uf + u;
                uvs[j][1] = vertices[idx + 1] * vf + v;
            }
            
            // Calculate face normal
            vec3 normal = vec3_face_normal(verts[2], verts[1], verts[0]);
            
            // Submit vertices in reverse order (for correct winding)
            r_push_vert(verts[2], normal, uvs[2][0], uvs[2][1]);
            r_push_vert(verts[1], normal, uvs[1][0], uvs[1][1]);
            r_push_vert(verts[0], normal, uvs[0][0], uvs[0][1]);
        }
    }
    
    r_models.push_back(model);
}

// Initialize all game models
void model_init_all() {
    // Define all models with their names and scales
    struct model_def {
        const char* name;
        float sx, sy, sz;
    };
    
    // Model definitions matching the JavaScript
    model_def defs[] = {
        {"shotgun", 1, 1, 1},
        {"nailgun", 1, 1, 1},
        {"grenadelauncher", 1, 1, 1},
        {"pickup_box", 1, 1, 1},
        {"pickup_key", 1, 1, 1},
        {"door", 1, 1, 1},
        {"barrel", 1, 1, 1},
        {"torch", 1, 1, 1},
        {"grunt", 1, 1, 1},
        {"enforcer", 1, 1, 1},
        {"ogre", 1, 1, 1},
        {"zombie", 1, 1, 1},
        {"hound", 1, 1, 1},
        {"explosion", 1, 1, 1},
        {"blood", 1, 1, 1},
        {"gib", 1, 1, 1},
        {"nail", 0.3, 0.3, 1},
        {"grenade", 0.5, 0.5, 0.5},
        {"plasma", 1, 1, 1}
    };
    
    // Note: In the actual implementation, we'd parse the loaded models
    // and assign them to the proper pointers based on order
}

void model_init(uint8_t* data, size_t size) {
    model_init(data, size, 1, 1, 1);
}