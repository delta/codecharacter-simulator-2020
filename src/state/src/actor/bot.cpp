/**
 * @file bot.cpp
 *  Definition of Bot class
 */

#include "state/actor/bot.h"
#include "state/actor/blaster.h"
#include "state/actor/bot_states/bot_idle_state.h"
#include <state/actor/bot_states/bot_blast_state.h>
#include <state/actor/bot_states/bot_dead_state.h>
#include <state/actor/bot_states/bot_move_state.h>
#include <state/actor/bot_states/bot_move_to_blast_state.h>
#include <state/actor/bot_states/bot_move_to_transform_state.h>
#include <state/actor/bot_states/bot_transform_state.h>

namespace state {

Bot::Bot(ActorId id, PlayerId player_id, size_t hp, size_t max_hp,
         DoubleVec2D position, size_t speed, size_t blast_range,
         size_t damage_points, ScoreManager *score_manager,
         PathPlanner *path_planner, BlastCallback blast_callback,
         ConstructTowerCallback construct_tower_callback)
    : Unit::Unit(id, player_id, ActorType::BOT, hp, max_hp, speed, position,
                 score_manager),
      Blaster::Blaster(blast_range, damage_points, std::move(blast_callback)),
      state(std::make_unique<BotIdleState>(this)), path_planner(path_planner),
      final_destination(DoubleVec2D::null), is_final_destination_set(false),
      construct_tower_callback(std::move(construct_tower_callback)),
      transform_destination(DoubleVec2D::null),
      is_transform_destination_set(false), is_transforming(false) {}

Bot::Bot(PlayerId player_id, size_t hp, size_t max_hp, DoubleVec2D position,
         size_t speed, size_t blast_range, size_t damage_points,
         ScoreManager *score_manager, PathPlanner *path_planner,
         BlastCallback blast_callback,
         ConstructTowerCallback construct_tower_callback)
    : Unit::Unit(player_id, ActorType::BOT, hp, max_hp, speed, position,
                 score_manager),
      Blaster::Blaster(blast_range, damage_points, std::move(blast_callback)),
      state(std::make_unique<BotIdleState>(this)), path_planner(path_planner),
      final_destination(DoubleVec2D::null), is_final_destination_set(false),
      construct_tower_callback(std::move(construct_tower_callback)),
      transform_destination(DoubleVec2D::null),
      is_transform_destination_set(false), is_transforming(false) {}

Bot::Bot(ActorId actor_id, PlayerId player_id, size_t hp, size_t max_hp,
         BotStateName bot_state_name, DoubleVec2D position, size_t speed,
         size_t blast_range, size_t damage_points, ScoreManager *score_manager,
         PathPlanner *path_planner, BlastCallback blast_callback,
         ConstructTowerCallback construct_tower_callback, bool is_blasting,
         bool is_transforming)
    : Unit::Unit(actor_id, player_id, ActorType::BOT, hp, max_hp, speed,
                 position, score_manager),
      Blaster::Blaster(blast_range, damage_points, is_blasting,
                       std::move(blast_callback)),
      path_planner(path_planner), final_destination(DoubleVec2D::null),
      is_final_destination_set(false),
      construct_tower_callback(std::move(construct_tower_callback)),
      transform_destination(DoubleVec2D::null),
      is_transform_destination_set(false), is_transforming(is_transforming) {

    switch (bot_state_name) {
    case BotStateName::IDLE:
        state = std::make_unique<BotIdleState>(this);
        break;
    case BotStateName::MOVE:
        state = std::make_unique<BotMoveState>(this);
        break;
    case BotStateName::MOVE_TO_BLAST:
        state = std::make_unique<BotMoveToBlastState>(this);
        break;
    case BotStateName::BLAST:
        state = std::make_unique<BotBlastState>(this);
        break;
    case BotStateName::TRANSFORM:
        state = std::make_unique<BotTransformState>(this);
        break;
    case BotStateName::MOVE_TO_TRANSFORM:
        state = std::make_unique<BotMoveToTransformState>(this);
        break;
    case BotStateName::DEAD:
        state = std::make_unique<BotDeadState>(this);
        break;
    }
}

void Bot::clearFinalDestination() {
    final_destination = DoubleVec2D::null;
    is_final_destination_set = false;
}

DoubleVec2D Bot::getFinalDestination() const { return final_destination; }

void Bot::clearTransformDestination() {
    transform_destination = DoubleVec2D::null;
    is_transform_destination_set = false;
}

DoubleVec2D Bot::getTransformDestination() const {
    return transform_destination;
}

bool Bot::isFinalDestinationSet() const { return is_final_destination_set; }

void Bot::setFinalDestination(DoubleVec2D p_final_destination) {
    clearDestination();
    clearTransformDestination();
    final_destination = p_final_destination;
    is_final_destination_set = true;
}

bool Bot::isTransformDestinationSet() const {
    return is_transform_destination_set;
}

void Bot::setTransformDestination(DoubleVec2D p_transform_destination) {
    clearDestination();
    clearFinalDestination();
    transform_destination = p_transform_destination;
    is_transform_destination_set = true;
}

void Bot::setTransforming(bool p_transforming) {
    is_transforming = p_transforming;
}

BotStateName Bot::getState() const { return state->getName(); }

void Bot::setState(BotStateName bot_state) {
    switch (bot_state) {
    case BotStateName::IDLE:
        state = std::make_unique<BotIdleState>(this);
        break;
    case BotStateName::MOVE:
        state = std::make_unique<BotMoveState>(this);
        break;
    case BotStateName::BLAST:
        state = std::make_unique<BotBlastState>(this);
        break;
    case BotStateName::TRANSFORM:
        state = std::make_unique<BotTransformState>(this);
        break;
    case BotStateName::MOVE_TO_BLAST:
        state = std::make_unique<BotMoveToBlastState>(this);
        break;
    case BotStateName::MOVE_TO_TRANSFORM:
        state = std::make_unique<BotMoveToTransformState>(this);
        break;
    case BotStateName::DEAD:
        state = std::make_unique<BotDeadState>(this);
        break;
    }
}

void Bot::constructTower() { construct_tower_callback(this); }

bool Bot::isTransforming() const { return is_transforming; }

void Bot::blast() {
    clearDestination();
    clearFinalDestination();
    clearTransformDestination();
    setBlasting(true);
}

void Bot::transform() {
    clearDestination();
    clearFinalDestination();
    clearTransformDestination();
    setTransforming(true);
}

PathPlanner *Bot::getPathPlanner() const { return path_planner; }

void Bot::lateUpdate() {
    // Updating the hp of the Bot
    setHp(getLatestHp());

    // Resetting the damage incurred
    setDamageIncurred(0);

    // Transition to dead state if dead
    if (getHp() == 0 && state->getName() != BotStateName::DEAD) {
        auto new_state = state->update();
        state->exit();
        state.reset(static_cast<BotState *>(new_state.release()));
        state->enter();
        return;
    }

    // perform a move
    if (isNewPostitionSet()) {
        DoubleVec2D previous_position = getPosition();
        TerrainType previous_terrain =
            path_planner->getTerrainType(previous_position);

        setPosition(getNewPosition());

        // Checking if the bot is has moved out of a flag from inside a flag
        DoubleVec2D position = getPosition();
        TerrainType terrain = path_planner->getTerrainType(position);
        if (previous_terrain == TerrainType::FLAG &&
            terrain != TerrainType::FLAG) {
            score_manager->actorExitedFlagArea(getActorType(), getPlayerId());
        }

        // Checking if the bot has moved into a flag from outside a flag
        if (previous_terrain != TerrainType::FLAG &&
            terrain == TerrainType::FLAG) {
            score_manager->actorEnteredFlagArea(getActorType(), getPlayerId());
        }

        clearNewPosition();
        return;
    }

    if (getHp() > 0 && isTransforming()) {
        constructTower();
    }
}

void Bot::update() {
    // get new state based on bot properties
    auto new_state = state->update();

    // until no state transitions occur, in a single frame
    while (new_state != nullptr) {
        // state transition occurred
        state->exit();
        /*
            Here, state.reset destroys the state it is currently managing and
           starts managing the new state object passed to it
        */
        state.reset(static_cast<BotState *>(new_state.release()));
        state->enter();

        new_state = state->update();
    }
}

} // namespace state
