/**
 * @file blaster.cpp
 * Defines the blaster class
 */

#include "state/actor/blaster.h"

namespace state {
Blaster::Blaster() {}

Blaster::Blaster(size_t blast_range, size_t damage_points,
                 BlastCallback blast_callback)
    : blast_range(blast_range), damage_points(damage_points), blasting(false),
      blast_callback(std::move(blast_callback)) {}

Blaster::Blaster(size_t blast_range, size_t damage_points, bool blasting,
                 BlastCallback blast_callback)
    : blast_range(blast_range), damage_points(damage_points),
      blasting(blasting), blast_callback(std::move(blast_callback)) {}

void Blaster::setBlastCallback(BlastCallback blast_callback) {
    this->blast_callback = blast_callback;
}

bool Blaster::isBlasting() const { return blasting; }

void Blaster::setBlasting(bool p_blasting) { blasting = p_blasting; }

void Blaster::damageEnemyActors(PlayerId player_id, ActorId actor_id,
                                DoubleVec2D position) {
    blast_callback(player_id, actor_id, position, getBlastDamage());
}

size_t Blaster::getBlastRange() const { return blast_range; }

size_t Blaster::getBlastDamage() const { return damage_points; }

void Blaster::setBlastRange(size_t p_blast_range) {
    blast_range = p_blast_range;
}

void Blaster::setBlastDamage(size_t p_blast_damage) {
    damage_points = p_blast_damage;
}

} // namespace state
