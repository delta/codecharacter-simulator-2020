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
     * @brief Update function of Bot/Tower
     */
    virtual void update() = 0;

    /**
     * @brief Late Update function of Bot/Tower. After the end of every turn.
     */
    virtual void lateUpdate() = 0;
};
} // namespace state
