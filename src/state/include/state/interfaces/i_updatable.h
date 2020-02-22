/**
 * @file i_updatable.h
 * Declares the updatable interfacs, which gives classs an update method
 */

#pragma once

#include "state/state_export.h"

namespace state {

/**
 * IUpdatable interface to define the Update method
 */
class STATE_EXPORT IUpdatable {
  public:
    virtual ~IUpdatable(){};

    /**
     * Update function of Bot/Tower
     */
    virtual void update() = 0;

    /**
     * Late Update function of Bot/Tower. At the end of every turn.
     */
    virtual void lateUpdate() = 0;
};
} // namespace state
