/**
 * @file blaster.cpp
 * Defines the blaster class
 */

#include "state/actor/blaster.h"

namespace state {
Blaster::Blaster() {}

Blaster::Blaster(size_t blast_range, size_t damage_points,
                 BlastCallback blast_callback)
    : blast_range(blast_range), damage_points(damage_points),
      blast_callback(std::move(blast_callback)), blasting(false) {}
void Blaster::setBlastCallback(BlastCallback blast_callback) {
    this->blast_callback = blast_callback;
}

bool Blaster::isBlasting() const { return blasting; }

void Blaster::setBlasting(bool p_blasting) { blasting = p_blasting; }

} // namespace state
