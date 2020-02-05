#include "state/actor/tower.h"
#include "state/map/map.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <memory>

using namespace std;
using namespace state;
using namespace testing;

namespace test {

// Mock function for blasting enemy actors
void blast_enemies(PlayerId player_id, DoubleVec2D pos) {}
class TowerTest : public Test {
  protected:
    unique_ptr<Tower> tower, enemy_tower;
    size_t hp, max_hp, damage_points, blast_range;

    BlastCallback blast_callback;

  public:
    TowerTest() {
        // Create a default map
        hp = 100;
        max_hp = 100;
        blast_range = 3;
        DoubleVec2D start_pos = DoubleVec2D(0, 0);

        blast_callback = &blast_enemies;

        tower = make_unique<Tower>(1, PlayerId::PLAYER1, ActorType::TOWER, hp,
                                   max_hp, start_pos, damage_points,
                                   blast_range, blast_callback);

        enemy_tower = make_unique<Tower>(1, PlayerId::PLAYER2, ActorType::TOWER,
                                         hp, max_hp, start_pos, damage_points,
                                         blast_range, blast_callback);
    }
};

TEST_F(TowerTest, BlastTest) {
    ASSERT_EQ(tower->getState(), TowerStateName::IDLE);

    // Making the tower blast and checking for the transition to the blast state
    // and then transition to the dead state
    tower->blast();

    // Check that blast is being set
    ASSERT_EQ(tower->isBlasting(), true);

    // getLatestHp returns hp - damage_incurred. If this is equal to 0, then
    // damage incurred has been set to hp
    ASSERT_EQ(tower->getLatestHp(), 0);

    // When the tower updates to blast state, it must call DamageEnemyActors to
    // increase all enemy actor's damage incurred
    /// TODO : Add a mock state object and check if state is being called to
    /// damage enemy units within blast range of given position

    // Check if the tower is in the blast state
    tower->update();
    ASSERT_EQ(tower->getState(), TowerStateName::BLAST);

    // Check if the tower is in the dead state
    tower->lateUpdate();
    ASSERT_EQ(tower->getState(), TowerStateName::DEAD);
    ASSERT_EQ(tower->getHp(), 0);

    // Checking if the tower's damage incurred is reset to 0
    ASSERT_EQ(tower->getDamageIncurred(), 0);
}

// Tower is attacked by neighbour, hence, his damage incurred is set
TEST_F(TowerTest, AttackTest) {
    // Setting the damage incurred of the tower
    ASSERT_EQ(tower->getState(), TowerStateName::IDLE);
    tower->setDamageIncurred(hp);

    // After update, the tower must still be in the idle state
    tower->update();
    ASSERT_EQ(tower->getState(), TowerStateName::IDLE);

    // After late update, the tower transitions into the dead state
    tower->lateUpdate();
    ASSERT_EQ(tower->getState(), TowerStateName::DEAD);
}

TEST_F(TowerTest, DeathTest) {
    ASSERT_EQ(tower->getState(), TowerStateName::IDLE);
    ASSERT_EQ(tower->getHp(), hp);

    // Setting the tower hp to 0 and checking for state transition to dead state
    tower->setHp(0);

    tower->update();
    ASSERT_EQ(tower->getState(), TowerStateName::DEAD);
}

} // namespace test
