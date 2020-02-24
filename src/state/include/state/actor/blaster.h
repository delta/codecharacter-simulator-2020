/**
 * @file blaster.h
 *  A class to expose blasting properties
 */

#pragma once

#include "state/actor/actor.h"

#include <functional>
#include <vector>

namespace state {

using BlastCallback =
    std::function<void(PlayerId player_id, ActorId actor_id,
                       DoubleVec2D position, uint64_t blast_range)>;

class STATE_EXPORT Blaster {
  protected:
    /**
     * All points for which the euclidean distance between blaster's position
     * and that position are within the blast range
     * The blast range is circular
     */
    uint64_t blast_range;

    /**
     * The damage that this blaster inflicts upon blasting
     */
    uint64_t damage_points;

    /**
     * True if the blaster is going to blast
     * False if the blaster isn't blasting
     */
    bool blasting;

    /**
     * Callback to implement effect of blaster blasting through state
     */
    BlastCallback blast_callback;

  public:
    /**
     * Constructors
     */
    Blaster();

    Blaster(uint64_t blast_range, uint64_t damage_points,
            BlastCallback blast_callback);

    /**
     * Method to set blast_callback
     *
     * @param blast_callback
     */
    void setBlastCallback(BlastCallback blast_callback);

    /**
     * The actor blasts and the actor's state transitions to dead state
     */
    virtual void blast() = 0;

    /**
     * Helper function to check if the blaster is going to blast in a given turn
     *
     * @return true if the blaster is blasting
     * @return false if the blaster is not blasting
     */
    bool isBlasting() const;

    /**
     * Helper function to set the blasting property of blaster
     */
    void setBlasting(bool p_blasting);

    /**
     * Function that uses callback function to damage enemy units due to
     * blasting
     *
     * @param player_id
     * @param actor_id
     * @param position
     */
    void damageEnemyActors(PlayerId player_id, ActorId actor_id,
                           DoubleVec2D position);

    /**
     *  Get the Blast Range value
     *
     * @return uint64_t blast_range
     */
    uint64_t getBlastRange() const;

    /**
     *  Get the Blast Damage value
     *
     * @return uint64_t damage_points
     */
    uint64_t getBlastDamage() const;
};

} // namespace state
