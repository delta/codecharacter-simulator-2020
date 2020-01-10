/**
 * @file command_giver.h
 * Validates user's actions and gives command taker the necessary valid instructions
 */

#pragma once

#include "interfaces/i_command_giver.h"
#include "interfaces/i_command_taker.h"
#include "player_state.h"
#include "vector.hpp"
// #include "logger/interfaces/i_logger.h"
#include "state.h"
#include "utilities.h"

namespace state{
    class STATE_EXPORT CommandGiver : public ICommandGiver{
        private:
            /**
             * Instance of overall state that player can't modify
             */
            std::unique_ptr<ICommandTaker> state; 

            /**
             * Instance of logger to log whenever the user makes invalid state transitions
             */
            //std::unique_ptr<ILogger> logger;

            /**
             * Helper function to validate blast request and make command_taker call BlastBot internally 
             */
            void BlastBot(PlayerId player_id, ActorId unit_id);

            /**
             * Helper function to validate transform request and make command_taker call TransformBot internally 
             */
            void TransformBot(PlayerId player_id, ActorId unit_id);

            /**
             * Helper function to use make command_taker call BlastBot internally 
             */
            void MoveBot(PlayerId player_id, int64_t bot_id,
                       physics::Vector<int64_t> position); 

            /**
             * Helper function to flip a position for enemy units  
             */
            DoubleVec2D FlipPosition(DoubleVec2D pos);

            /**
             * Helper function to check if given position is within the map 
             */
            bool IsValidPosition(DoubleVec2D pos) const;

            /**
             * Helper function to check whether the offset given is within the map 
             */
            bool IsValidOffset(DoubleVec2D pos) const;

            /**
             * Helper function to check whether a position has already been occupied by a wall
             * 
             * If occupied, returns actor id of that factory through occupied_actor_id
             */
            // bool IsOccupied(int64_t player_id, Vec2D offset,
            //                 std::array<std::vector<Wall *>, 2> state_walls,
            //             ActorId &occupied_actor_id);

        public:
            CommandGiver(); 

            // TODO : Need to add logger
            CommandGiver(std::unique_ptr<ICommandTaker> state);

            /**
             * Validates the actions taken by the player and if valid, makes calls to command taker to make state changes  
             */
            void RunCommands(std::array<player_state::State, 2> player_states,
                           std::array<bool, 2> skip_turns);
    };
}
