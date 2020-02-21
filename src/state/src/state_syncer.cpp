/**
 * @file state_syncer.cpp
 */

#include "state/state_syncer.h"

namespace state {
StateSyncer::StateSyncer() {}

StateSyncer::StateSyncer(std::unique_ptr<ICommandTaker> state,
                         std::unique_ptr<ICommandGiver> command_giver,
                         std::unique_ptr<logger::ILogger> logger)
    : state(std::move(state)), command_giver(std::move(command_giver)),
      logger(std::move(logger)) {}

void StateSyncer::updateMainState(
    std::array<player_state::State, 2> &player_states,
    std::array<bool, 2> skip_turns) {
    // Running the user's commands
    command_giver->runCommands(player_states, skip_turns);

    // Updating the main state
    state->Update();

    // Updating the player states
    updatePlayerStates(player_states);
}

bool StateSyncer::isGameOver(PlayerId &winner) {
    return state->isGameOver(winner);
}

size_t StateSyncer::getPlayerId(size_t player_id, bool is_enemy) const {
    if (is_enemy) {
        return (player_id + 1) % (static_cast<size_t>(PlayerId::PLAYER_COUNT));
    }
    return static_cast<size_t>(player_id);
}

// TODO : Fill the following functions after the merging of tower and bot
void StateSyncer::updatePlayerStates(
    std::array<player_state::State, 2> &player_states) {}

void StateSyncer::assignBots(std::vector<player_state::Bot> player_bots,
                             bool is_enemy) {}

void StateSyncer::assignTowers(std::vector<player_state::Tower> player_towers,
                               bool is_enemy) {}

} // namespace state
