#include "entity_trigger_level.h"
#include "entity_player.h"
#include "game.h"

entity_trigger_level_t::entity_trigger_level_t(const vec3& pos, void* p1, void* p2) 
    : entity_t(pos, p1, p2) {
}

void entity_trigger_level_t::_init(void* /*p1*/, void* /*p2*/) {
    // Trigger has no visual representation
    s = vec3(64, 64, 64);  // Trigger volume size
}

void entity_trigger_level_t::_update() {
    if (!_dead && game_entity_player && vec3_dist(p, game_entity_player->p) < 64) {
        game_next_level();
        _dead = true;
    }
}

void entity_trigger_level_t::_did_collide_with_entity(EntityPtr other) {
    // Only trigger for player
    if (!_dead && other == game_entity_player) {
        game_next_level();
        _dead = true;
    }
}