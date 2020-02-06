#include "state/actor/bot.h"
#include "state/map/map.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <memory>

using namespace std;
using namespace state;
using namespace testing;

namespace test {

// Mock function declaration for blasting enemy actors
// Defined in test namespace in tower_test.cpp
void blast_enemies(PlayerId player_id, ActorId actor_id, DoubleVec2D position);

// Mock function defintion for transforming bot to tower
void construct_tower(PlayerId player_id, DoubleVec2D position) {}

class BotTest : public Test {
  protected:
    unique_ptr<Bot> bot, enemy_bot;
    size_t hp, max_hp, damage_points, blast_range, speed;

    BlastCallback blast_callback;
    ConstructTowerCallback construct_tower_callback;

  public:
    BotTest() {
        hp = 150;
        max_hp = 150;
        blast_range = 5;
        speed = 3;
        DoubleVec2D start_position = DoubleVec2D(0, 0);

        blast_callback = &blast_enemies;
        construct_tower_callback = &construct_tower;

        bot =
            make_unique<Bot>(1, PlayerId::PLAYER1, ActorType::BOT, hp, max_hp,
                             start_position, speed, blast_range, damage_points,
                             blast_callback, construct_tower_callback);

        enemy_bot =
            make_unique<Bot>(2, PlayerId::PLAYER1, ActorType::BOT, hp, max_hp,
                             start_position, speed, blast_range, damage_points,
                             blast_callback, construct_tower_callback);
    }
};

TEST_F(BotTest, MoveTest) {
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);

    //  making the bot move to {1, 1}, and checking for move state transition
    auto destination_position = DoubleVec2D(1, 0);
    bot->move(destination_position);

    // check movement related properties
    ASSERT_EQ(bot->isDestinationSet(), true);
    ASSERT_EQ(bot->getDestination(), destination_position);

    // updating bot
    bot->update();

    // check for state transition
    ASSERT_EQ(bot->getState(), BotStateName::MOVE);
    // bot should not have reached position yet
    ASSERT_NE(bot->getPosition(), destination_position);

    bot->update();

    // bot moves closer to destination

    // TODO: check bot position and destination position, newPosition
    // TODO: check transition to IDLE state
}

TEST_F(BotTest, BlastTest) {
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);

    // Making the bot blast and checking for the transition to the blast state
    // and then transition to the dead state
    bot->blast();

    // Check that blast is being set
    ASSERT_EQ(bot->isBlasting(), true);

    // getLatestHp returns hp - damage_incurred. If this is equal to 0, then
    // damage incurred has been set to hp
    ASSERT_EQ(bot->getLatestHp(), 0);

    // When the bot updates to blast state, it must call DamageEnemyActors to
    // increase all enemy actor's damage incurred
    // TODO : Add a mock state object and check if state is being called to
    // damage enemy units within blast range of given position

    // Check if the bot is in the blast state
    bot->update();
    ASSERT_EQ(bot->getState(), BotStateName::BLAST);

    // Check if the bot is in the dead state
    bot->lateUpdate();
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->getHp(), 0);

    // Checking if the bot's damage incurred is reset to 0
    ASSERT_EQ(bot->getDamageIncurred(), 0);
}

TEST_F(BotTest, MoveToBlastTest) {
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);

    //  making the bot move to {1, 1}, and checking for move state transition
    auto blow_up_position = DoubleVec2D(1, 1);
    bot->setFinalDestination(blow_up_position);

    // check move to blast related properties
    ASSERT_EQ(bot->isFinalDestinationSet(), true);
    ASSERT_EQ(bot->getFinalDestination(), blow_up_position);

    // updating bot to transition state from idle to move to blast
    bot->update();

    // check for state transition
    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_BLAST);
    // bot should not have reached position yet
    ASSERT_NE(bot->getPosition(), blow_up_position);

    // state transitions to blast if destination is blow_up_destination or
    // remains in move to blast state
    bot->update();

    // TODO: check bot position and final_destination position
    // TODO: check for isBlasting
    // TODO: check transition to Blast state.
    // TODO: see #BlastTest
}

// Bot is attacked by neighbour, hence, his damage incurred is set
TEST_F(BotTest, AttackTest) {
    // Setting the damage incurred of the bot
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);
    bot->setDamageIncurred(hp);

    // After update, the bot must still be in the idle state
    bot->update();
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);

    // After late update, the bot transitions into the dead state
    bot->lateUpdate();
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
}

TEST_F(BotTest, DeathTest) {
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);
    ASSERT_EQ(bot->getHp(), hp);

    // Setting the bot hp to 0 and checking for state transition to dead state
    bot->setHp(0);

    bot->update();
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
}

TEST_F(BotTest, TransformTest) {
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);

    //  making the bot transform into a tower
    bot->transform();

    // check transfom related properties
    ASSERT_EQ(bot->isTransforming(), true);

    // updating bot to transition state from idle to transform
    bot->update();

    // TODO: find a way to test creation of new tower instance at bot position

    // check for state transition, from idle to transform
    ASSERT_EQ(bot->getState(), BotStateName::TRANSFORM);

    // state transition to go to dead state
    bot->lateUpdate();

    // bot is dead
    ASSERT_EQ(bot->getState(), BotStateName::DEAD);
    ASSERT_EQ(bot->getHp(), 0);
}

TEST_F(BotTest, MoveToTransformTest) {
    ASSERT_EQ(bot->getState(), BotStateName::IDLE);

    //  making the bot move to transform to tower
    auto transform_to_tower_destination = DoubleVec2D(1, 1);

    bot->setTransformDestination(transform_to_tower_destination);

    // check transfom to blast related properties
    ASSERT_EQ(bot->isTransformDestinationSet(), true);
    ASSERT_EQ(bot->getTransformDestination(), transform_to_tower_destination);

    // updating bot to transition state from idle to move to transfom
    bot->update();

    // state transition
    ASSERT_EQ(bot->getState(), BotStateName::MOVE_TO_TRANSFORM);
    // bot should not have reached the transform destination
    ASSERT_NE(bot->getPosition(), transform_to_tower_destination);

    // Either new position is set, or state transitions to transform state
    bot->update();

    // TODO: see #TransformTest
}

} // namespace test
