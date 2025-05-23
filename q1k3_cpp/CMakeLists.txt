cmake_minimum_required(VERSION 3.16)
project(Q1K3_CPP VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/vec3.cpp
    src/core/math_utils.cpp
    src/game/game.cpp
    src/game/entity.cpp
    src/game/entity_player.cpp
    src/game/entity_light.cpp
    src/game/entity_particle.cpp
    src/game/entity_projectile.cpp
    src/game/entity_enemy.cpp
    src/game/entity_pickup.cpp
    src/game/entity_door.cpp
    src/game/entity_barrel.cpp
    src/game/entity_torch.cpp
    src/game/entity_trigger_level.cpp
    src/game/weapons.cpp
    src/game/audio.cpp
    src/game/ui.cpp
    src/game/timer.cpp
    src/platform/platform.cpp
    src/platform/input.cpp
    src/renderer/renderer.cpp
    src/renderer/ttt.cpp
    src/renderer/texture.cpp
    src/renderer/model.cpp
    src/assets/map.cpp
)

# Create executable
add_executable(q1k3 ${SOURCES})

# Link libraries
target_link_libraries(q1k3
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    ${SDL2_LIBRARIES}
)

# Copy assets to build directory
add_custom_command(TARGET q1k3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../assets ${CMAKE_BINARY_DIR}/assets)

# Compiler flags
if(MSVC)
    target_compile_options(q1k3 PRIVATE /W4)
else()
    target_compile_options(q1k3 PRIVATE -Wall -Wextra -g -O0)  # Debug symbols, no optimization
endif()