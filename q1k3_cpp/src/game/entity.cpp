#include "entity.h"
#include <cmath>
#include <algorithm>

entity_t::entity_t(const vec3& pos, void* p1, void* p2) 
    : a(), v(), p(pos), s(2, 2, 2), f(0),
      _health(50), _dead(false), _die_at(0), _step_height(0),
      _bounciness(0), _gravity(1), _yaw(0), _pitch(0),
      _anim({1, {0}}), _anim_time(static_cast<float>(rand()) / RAND_MAX),
      _on_ground(false), _keep_off_ledges(false),
      _check_against(ENTITY_GROUP_NONE), _stepped_up_at(0),
      _model(nullptr), _texture(0) {
    
    _init(p1, p2);
}

void entity_t::_update() {
    if (_model) {
        _draw_model();
    }
}

void entity_t::_update_physics() {
    if (_die_at && _die_at < game_time) {
        _kill();
        return;
    }

    // Apply Gravity
    a.y = -1200 * _gravity;

    // Integrate acceleration & friction into velocity
    float ff = std::min(f * game_tick, 1.0f);
    v = v + (a * game_tick - v * vec3(ff, 0, ff));

    // Set up the _check_entities array for entity collisions
    switch (_check_against) {
        case ENTITY_GROUP_NONE:
            _check_entities.clear();
            break;
        case ENTITY_GROUP_PLAYER:
            _check_entities = game_entities_friendly;
            break;
        case ENTITY_GROUP_ENEMY:
            _check_entities = game_entities_enemies;
            break;
    }

    // Divide the physics integration into 16 unit steps
    float original_step_height = _step_height;
    vec3 move_dist = v * game_tick;
    int steps = std::ceil(vec3_length(move_dist) / 16.0f);
    vec3 move_step = move_dist * (1.0f / steps);

    for (int s = 0; s < steps; s++) {
        vec3 lp = vec3_clone(p);
        p = p + move_step;

        // Collision with walls, horizontal
        if (_collides(vec3(p.x, lp.y, lp.z))) {
            if (!_step_height || !_on_ground || v.y > 0 || 
                _collides(vec3(p.x, lp.y + _step_height, lp.z))) {
                _did_collide(0);
                p.x = lp.x;
                v.x = -v.x * _bounciness;
            } else {
                lp.y += _step_height;
                _stepped_up_at = game_time;
            }
            s = steps;
        }

        // Collision with walls, vertical
        if (_collides(vec3(p.x, lp.y, p.z))) {
            if (!_step_height || !_on_ground || v.y > 0 || 
                _collides(vec3(p.x, lp.y + _step_height, p.z))) {
                _did_collide(2);
                p.z = lp.z;
                v.z = -v.z * _bounciness;
            } else {
                lp.y += _step_height;
                _stepped_up_at = game_time;
            }
            s = steps;
        }

        // Collision with ground/ceiling
        if (_collides(p)) {
            _did_collide(1);
            p.y = lp.y;

            float bounce = std::abs(v.y) > 200 ? _bounciness : 0;
            _on_ground = v.y < 0 && !bounce;
            v.y = -v.y * bounce;
            s = steps;
        }

        _step_height = original_step_height;
    }
}

bool entity_t::_collides(const vec3& p) {
    if (_dead) {
        return false;
    }

    for (auto& entity : _check_entities) {
        if (vec3_dist(p, entity->p) < s.y + entity->s.y) {
            _step_height = 0;
            _did_collide_with_entity(entity);
            return true;
        }
    }

    // Check if there's no block beneath this point
    if (_on_ground && _keep_off_ledges &&
        !map_block_at(p.x / 32, (p.y - s.y - 8) / 16, p.z / 32) &&
        !map_block_at(p.x / 32, (p.y - s.y - 24) / 16, p.z / 32)) {
        return true;
    }

    return map_block_at_box(p - s, p + s);
}

void entity_t::_draw_model() {
    _anim_time += game_tick;

    float f = _anim_time / _anim.first;
    float mix = f - static_cast<int>(f);
    int frame_cur = _anim.second[static_cast<int>(f) % _anim.second.size()];
    int frame_next = _anim.second[static_cast<int>(f + 1) % _anim.second.size()];

    if (frame_next < frame_cur) {
        std::swap(frame_next, frame_cur);
        mix = 1 - mix;
    }

    r_draw(p, _yaw, _pitch, _texture,
           _model->f[frame_cur], _model->f[frame_next], mix,
           _model->nv);
}

void entity_t::_spawn_particles(int amount, float speed, model_t* model, int texture, float lifetime) {
    for (int i = 0; i < amount; i++) {
        auto particle = game_spawn<entity_particle_t>(p);
        particle->_model = model;
        particle->_texture = texture;
        particle->_die_at = game_time + lifetime + static_cast<float>(rand()) / RAND_MAX * lifetime * 0.2f;
        particle->v = vec3(
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * speed,
            static_cast<float>(rand()) / RAND_MAX * speed,
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * speed
        );
    }
}

void entity_t::_receive_damage(EntityPtr from, float amount) {
    if (_dead) {
        return;
    }
    _health -= amount;
    if (_health <= 0) {
        _kill();
    }
}

void entity_t::_play_sound(void* sound) {
    float volume = clamp(scale(vec3_dist(p, r_camera), 64, 1200, 1, 0), 0, 1);
    float pan = std::sin(vec3_2d_angle(p, r_camera) - r_camera_yaw) * -1;
    audio_play(sound, volume, 0, pan);
}

void entity_t::_kill() {
    _dead = true;
}