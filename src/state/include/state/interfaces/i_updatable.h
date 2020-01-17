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

<<<<<<< 4a375c0fa5a5d5ff3fec7b521f15ca9a29d582db
    virtual void Update() = 0;
=======
  virtual void Update() = 0;

  virtual void LateUpdate() = 0;
>>>>>>> Add i_actor_state interface
};
} // namespace state
