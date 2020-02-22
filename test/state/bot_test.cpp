#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/map/map.h"
#include "gtest/gtest.h"
#include <memory>

using namespace std;
using namespace state;
using namespace testing;

const size_t MAP_SIZE = 5;
const auto L = state::TerrainType::LAND;
const auto W = state::TerrainType::WATER;
const auto F = state::TerrainType::FLAG;

const size_t BOT_HP = 150;
const size_t BOT_BLAST_RANGE = 5;
const size_t BOT_DAMAGE_POINTS = 0;
const size_t BOT_SPEED = 3;
const auto START_POS = DoubleVec2D(0, 0);

namespace test {

// Mock function declaration for blasting enemy actors
// Defined in test namespace in tower_test.cpp
void blast_enemies(PlayerId player_id, ActorId actor_id, DoubleVec2D position,
                   size_t blast_range);

// Mock function definition for transforming bot to tower
void construct_tower(Bot *) {}

class BotTest : public Test {
  protected:
    unique_ptr<Bot> bot;
    size_t hp, max_hp, damage_points, blast_range, speed;
    unique_ptr<PathPlanner> path_planner;
    BlastCallback blast_callback;
    ConstructTowerCallback construct_tower_callback;

  public:
    BotTest() {
        hp = BOT_HP;
        max_hp = BOT_HP;
        blast_range = BOT_BLAST_RANGE;
        damage_points = BOT_DAMAGE_POINTS;
        speed = BOT_SPEED;
        DoubleVec2D start_position = START_POS;
        auto map_matrix = vector<vector<state::TerrainType>>{

            // clang-format off
            /*0  1  2  3  4 */
            {{ L, L, L, L, L }, // 0
             { L, L, L, L, L }, // 1
             { L, L, L, L, L }, // 2
             { L, L, L, L, L }, // 3
             { L, L, L, L, L }} // 4
        };
        // clang-format on

        auto map = make_unique<Map>(map_matrix, MAP_SIZE);
        path_planner = make_unique<PathPlanner>(std::move(map));
        path_planner->recomputePathGraph();
        blast_callback = &blast_enemies;
        construct_tower_callback = &construct_tower;

        bot = make_unique<Bot>(1, PlayerId::PLAYER1, hp, max_hp, start_position,
                               speed, blast_range, damage_points,
                               path_planner.get(), blast_callback,
                               construct_tower_callback);
    }
};

TEST_F(BotTest, InitialValuesTest) {
    // check all member values for new bot instance
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);
    ASSERT_EQ(bot->getHp(), BOT_HP);
    ASSERT_EQ(bot->getMaxHp(), BOT_HP);
    ASSERT_EQ(bot->getBlastRange(), BOT_BLAST_RANGE);
    ASSERT_EQ(bot->getBlastDamage(), 0);
    ASSERT_EQ(bot->getSpeed(), BOT_SPEED);
    ASSERT_EQ(bot->getPosition(), START_POS);
    ASSERT_EQ(bot->getDestination(), DoubleVec2D::null);
    ASSERT_EQ(bot->isDestinationSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getFinalDestination(), DoubleVec2D::null);
    ASSERT_EQ(bot->isFinalDestinationSet(), false);
    ASSERT_EQ(bot->isTransformDestinationSet(), false);
    ASSERT_EQ(bot->getFinalDestination(), DoubleVec2D::null);
    ASSERT_EQ(bot->isBlasting(), false);
    ASSERT_EQ(bot->isTransforming(), false);
}

TEST_F(BotTest, MoveTest) {
    // define local variables
    const auto INTERMEDIATE_POS = DoubleVec2D(3, 0);
    const auto DESTINATION_POS = DoubleVec2D(4, 0);

    // make the bot move to destination, and check for move state transition
    bot->move(DESTINATION_POS);

    // check movement related properties
    ASSERT_EQ(bot->isDestinationSet(), true);
    ASSERT_EQ(bot->getDestination(), DESTINATION_POS);

    // state transition from idle to move
    bot->update();

    ASSERT_EQ(bot->getState(), BotStateName::MOVE);
    ASSERT_EQ(bot->isNewPostitionSet(), true);
    ASSERT_EQ(bot->getNewPosition(), INTERMEDIATE_POS);

    // move bot to interim position
    bot->lateUpdate();

    // check for state transition
    ASSERT_EQ(bot->getState(), BotStateName::MOVE);
    ASSERT_EQ(bot->getPosition(), INTERMEDIATE_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);

    // state transitions
    bot->update();

    ASSERT_EQ(bot->getState(), BotStateName::IDLE);
    ASSERT_EQ(bot->isNewPostitionSet(), true);
    ASSERT_EQ(bot->getNewPosition(), DESTINATION_POS);

    // move bot to destination
    bot->lateUpdate();

    ASSERT_EQ(bot->getState(), BotStateName::IDLE);
    ASSERT_EQ(bot->getPosition(), DESTINATION_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isDestinationSet(), false);
    ASSERT_EQ(bot->getDestination(), DoubleVec2D::null);
}

TEST_F(BotTest, BlastTest) {
    // Make the bot blast and check for the transition to the blast state
    // and then transition to the dead state
    bot->blast();

    // Check that blast is being set
    ASSERT_EQ(bot->isBlasting(), true);

    // getLatestHp returns hp - damage_incurred. If this is equal to 0, then
    // damage incurred has been set to hp
    ASSERT_EQ(bot->getLatestHp(), BOT_HP);

    // When the bot updates to blast state, it must call DamageEnemyActors to
    // increase all enemy actor's damage incurred
    // TODO : Add a mock state object and check if state is being called to
    // damage enemy units within blast range of given position

    // TODO: check case where two bots blast at the same frame, within blast
    // range of each other

    // TODO: check above case with enemy bot and ally bot

    // Check if the bot is in the blast state
    bot->update();
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);

    // Check if the bot is in the dead state
    bot->lateUpdate();
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->getHp(), 0);

    // Checking if the bot's damage incurred is reset to 0
    ASSERT_EQ(bot->getDamageIncurred(), 0);
}

TEST_F(BotTest, MoveToBlastTest) {
    // define local variables
    const auto INTERMEDIATE_POS = DoubleVec2D(3, 0);
    const auto BLOW_UP_POS = DoubleVec2D(4, 0);

    // make the bot move to position before blasting
    bot->setFinalDestination(BLOW_UP_POS);

    // check move to blast related properties
    ASSERT_EQ(bot->isFinalDestinationSet(), true);
    ASSERT_EQ(bot->getFinalDestination(), BLOW_UP_POS);

    // update bot state to transition from idle to move to blast
    bot->update();

    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_BLAST);
    ASSERT_EQ(bot->isNewPostitionSet(), true);
    ASSERT_EQ(bot->getNewPosition(), INTERMEDIATE_POS);
    ASSERT_EQ(bot->isBlasting(), false);

    bot->lateUpdate();

    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_BLAST);
    ASSERT_EQ(bot->getPosition(), INTERMEDIATE_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isBlasting(), false);

    // update bot state to reach destination, set isBlasting and remain in move
    // to blast state
    bot->update();

    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_BLAST);
    ASSERT_EQ(bot->isNewPostitionSet(), true);
    ASSERT_EQ(bot->getNewPosition(), BLOW_UP_POS);
    ASSERT_EQ(bot->isFinalDestinationSet(), true);
    ASSERT_EQ(bot->getFinalDestination(), BLOW_UP_POS);

    // check if blast characteristics are turned off
    ASSERT_EQ(bot->isBlasting(), false);

    // perform move
    bot->lateUpdate();

    ASSERT_EQ(bot->isBlasting(), false);
    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_BLAST);
    ASSERT_EQ(bot->getPosition(), BLOW_UP_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isFinalDestinationSet(), true);
    ASSERT_EQ(bot->getFinalDestination(), BLOW_UP_POS);

    // bot at blast position and ready to blast
    // set isblasting to true and transition to blast state, ultimately ending
    // in dead state
    bot->update();

    // bot blasted and is now dead
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->isBlasting(), false);
    ASSERT_EQ(bot->getPosition(), BLOW_UP_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isFinalDestinationSet(), false);
    ASSERT_EQ(bot->getFinalDestination(), DoubleVec2D::null);

    bot->lateUpdate();

    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->getPosition(), BLOW_UP_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isFinalDestinationSet(), false);
    ASSERT_EQ(bot->getFinalDestination(), DoubleVec2D::null);
    ASSERT_EQ(bot->isBlasting(), false);
}

// Bot is attacked by neighbour, hence, his damage incurred is set
TEST_F(BotTest, AttackTest) {
    // Setting the damage incurred of the bot
    bot->setDamageIncurred(hp);

    // After update, the bot must still be in the idle state
    bot->update();
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);

    // After late update, the bot transitions into the dead state
    bot->lateUpdate();
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
}

TEST_F(BotTest, DeathTest) {
    ASSERT_EQ(bot->getHp(), hp);

    // Setting the bot hp to 0 and checking for state transition to dead state
    bot->setHp(0);

    bot->update();
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
}

TEST_F(BotTest, TransformTest) {
    //  make the bot transform into a tower
    bot->transform();

    // check transform related properties
    ASSERT_EQ(bot->isTransforming(), true);

    // updating bot to transition state from idle to transform
    bot->update();

    // TODO: find a way to test creation of new tower instance at bot position

    // check for state transition, from idle to transform to dead state
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->isTransforming(), false);

    bot->lateUpdate();

    // bot is in dead state, but hp not 0
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_NE(bot->getHp(), 0);
    ASSERT_EQ(bot->isTransforming(), false);
}

TEST_F(BotTest, MoveToTransformTest) {
    // define local variables
    const auto INTERMEDIATE_POS = DoubleVec2D(3, 0);
    const auto TRANSFORM_POS = DoubleVec2D(4, 0);

    //  make the bot move to transform to tower
    bot->setTransformDestination(TRANSFORM_POS);

    // check move to transform related properties
    ASSERT_EQ(bot->isTransformDestinationSet(), true);
    ASSERT_EQ(bot->getTransformDestination(), TRANSFORM_POS);

    // updating bot to transition state from idle to move to transfom
    bot->update();

    // state transition
    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_TRANSFORM);
    ASSERT_EQ(bot->isNewPostitionSet(), true);
    ASSERT_EQ(bot->getNewPosition(), INTERMEDIATE_POS);

    bot->lateUpdate();

    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_TRANSFORM);
    ASSERT_EQ(bot->getPosition(), INTERMEDIATE_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);

    // update bot state to reach transform destination, remain in move to
    // transform state and turn isTransforming off
    bot->update();

    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_TRANSFORM);
    ASSERT_EQ(bot->isNewPostitionSet(), true);
    ASSERT_EQ(bot->getNewPosition(), TRANSFORM_POS);
    ASSERT_EQ(bot->isTransforming(), false);

    // check if move to transform characteristics are not turned off
    ASSERT_EQ(bot->isTransformDestinationSet(), true);
    ASSERT_EQ(bot->getTransformDestination(), TRANSFORM_POS);

    // move bot to transform position
    bot->lateUpdate();

    ASSERT_EQ(bot->isTransforming(), false);
    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_TRANSFORM);
    ASSERT_EQ(bot->getPosition(), TRANSFORM_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isTransformDestinationSet(), true);
    ASSERT_EQ(bot->getTransformDestination(), TRANSFORM_POS);

    // bot at transform position and ready to transform
    // set istransform to true and perform state transitions
    bot->update();

    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->isTransforming(), false);
    ASSERT_EQ(bot->getPosition(), TRANSFORM_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isTransformDestinationSet(), false);
    ASSERT_EQ(bot->getTransformDestination(), DoubleVec2D::null);

    // call construct tower callback
    bot->lateUpdate();

    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->getPosition(), TRANSFORM_POS);
    ASSERT_EQ(bot->isNewPostitionSet(), false);
    ASSERT_EQ(bot->getNewPosition(), DoubleVec2D::null);
    ASSERT_EQ(bot->isTransformDestinationSet(), false);
    ASSERT_EQ(bot->getTransformDestination(), DoubleVec2D::null);
    ASSERT_EQ(bot->isTransforming(), false);
}

} // namespace test
