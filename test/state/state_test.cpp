#include "state/state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;
using namespace state;

const auto L = state::TerrainType::LAND;
const auto W = state::TerrainType::WATER;
const auto F = state::TerrainType::FLAG;
const auto T = state::TerrainType::TOWER;

class StateTest : public Test {
  public:
    unique_ptr<State> state;
    size_t map_size;
    int64_t bot_max_hp, tower_max_hp;

    StateTest() {

        // std::cerr << "Starting state tests\n\n\n\n\n\n\n\n";
        map_size = 5;

        auto score_manager = make_unique<ScoreManager>();

        // clang-format off
        auto test_map = vector<vector<TerrainType>>{
            {T, L, L, L, W}, 
            {L, L, L, L, L},
            {L, L, L, L, L},
            {L, L, L, F, L},
            {W, L, L, L, T},
        };
        // clang-format on

        auto map = make_unique<Map>(test_map, map_size);
        auto path_planner = make_unique<PathPlanner>(map.get());

        // Creating bots and towers
        int64_t bot_hp = 100, speed = 5, bot_blast_range = 3,
                bot_damage_points = 30;
        bot_max_hp = 100;

        auto state_bots = array<vector<unique_ptr<Bot>>, 2>{};
        auto state_towers = array<vector<unique_ptr<Tower>>, 2>{};

        state_bots[0].push_back(make_unique<Bot>(
            1, PlayerId::PLAYER1, bot_hp, bot_max_hp, DoubleVec2D(1, 1), speed,
            bot_blast_range, bot_damage_points, score_manager.get(),
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{}));
        state_bots[1].push_back(make_unique<Bot>(
            2, PlayerId::PLAYER2, bot_hp, bot_max_hp, DoubleVec2D(4, 4), speed,
            bot_blast_range, bot_damage_points, score_manager.get(),
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{}));

        int64_t tower_hp = 250, tower_blast_range = 5, tower_damage_points = 50;
        tower_max_hp = 250;

        state_towers[0].push_back(make_unique<Tower>(
            PlayerId::PLAYER1, tower_hp, tower_max_hp, DoubleVec2D(0, 0),
            tower_damage_points, tower_blast_range, score_manager.get(),
            BlastCallback{}));
        state_towers[1].push_back(make_unique<Tower>(
            PlayerId::PLAYER2, tower_hp, tower_max_hp, DoubleVec2D(4, 4),
            tower_damage_points, tower_blast_range, score_manager.get(),
            BlastCallback{}));

        state = make_unique<State>(move(map), move(score_manager),
                                   move(path_planner), move(state_bots),
                                   move(state_towers));
    }
};

TEST_F(StateTest, GetActors) {
    auto bots = state->getBots();
    auto towers = state->getTowers();

    ASSERT_EQ(bots[0].size(), 1);
    ASSERT_EQ(bots[1].size(), 1);
    ASSERT_EQ(towers[0].size(), 1);
    ASSERT_EQ(towers[1].size(), 1);
}

TEST_F(StateTest, GetMap) {
    auto map = state->getMap();
    ASSERT_EQ(!map, false);

    size_t map_size = map->getSize();
    ASSERT_EQ(map_size, 5);
    size_t p1 = 0, p2 = 3;
    ASSERT_EQ(map->getTerrainType(p1, p1), TerrainType::TOWER);
    ASSERT_EQ(map->getTerrainType(p2, p2), TerrainType::FLAG);
}

TEST_F(StateTest, MoveBotTest) {
    auto bots = state->getBots();
    auto bot = bots[0][0];
    ASSERT_EQ(bot->isDestinationSet(), false);

    // Calling move bot in state and checking if bots destination is set
    state->moveBot(1, DoubleVec2D(2, 2));

    // Getting PLAYER1's first bot and checking if his destination is set
    DoubleVec2D bot_destination = bot->getDestination();
    ASSERT_EQ(bot->isDestinationSet(), true);
    ASSERT_EQ(bot_destination, DoubleVec2D(2, 2));
}

TEST_F(StateTest, CreateTowerTest) {
    // Transforming the first bot of PLAYER2 into a tower
    auto bots = state->getBots();
    auto bot = bots[1][0];
    auto score_manager = state->getScoreManager();

    int64_t bot_id = bot->getActorId();
    int64_t hp = 50;
    bot->setHp(50);

    ASSERT_EQ(bots[0].size(), 1);
    ASSERT_EQ(bots[1].size(), 1);

    bot->setDestination(DoubleVec2D(3.5, 3.5));
    bot->update();
    bot->lateUpdate();

    // Checking initial number of bots on flags
    auto bot_counts = score_manager->getBotCounts();
    ASSERT_EQ(bot_counts[1], 1);

    // Calling create tower on this bot
    state->createTower(bot);

    bots = state->getBots();
    auto towers = state->getTowers();

    // Checking to see if the bot was removed from state and if a tower was
    // added to state
    ASSERT_EQ(bots[1].size(), 1);
    ASSERT_EQ(towers[1].size(), 2);

    // Checking to see if the tower has the same id as the bot
    auto tower = towers[1][1];
    ASSERT_EQ(tower->getActorId(), bot_id);

    // Check if the tower HP has been scaled correctly
    double hp_ratio = double(tower_max_hp) / double(bot_max_hp);

    int64_t tower_hp = hp * hp_ratio;
    ASSERT_EQ(tower->getHp(), tower_hp);

    // Checking score_manager counts because the bot was on a FLAG
    bot_counts = score_manager->getBotCounts();
    auto tower_counts = score_manager->getTowerCounts();
    ASSERT_EQ(tower_counts[1], 1);
    ASSERT_EQ(bot_counts[1], 0);
}

TEST_F(StateTest, TransformRequestTest) {
    auto bots = state->getBots();
    auto bot = bots[0][0];
    auto actor_id = bot->getActorId();
    auto player_id = bot->getPlayerId();
    auto position = bot->getPosition();

    auto transform_requests = state->getTransformRequests();
    ASSERT_EQ(transform_requests[0].size(), 0);
    ASSERT_EQ(transform_requests[1].size(), 0);

    state->transformBot(player_id, actor_id, position);

    transform_requests = state->getTransformRequests();
    ASSERT_EQ(transform_requests[0].size(), 1);
    ASSERT_EQ(transform_requests[1].size(), 0);

    auto transform_request = transform_requests[0][0];
    ASSERT_EQ(transform_request->getPlayerId(), player_id);
    ASSERT_EQ(transform_request->getBotId(), actor_id);
    ASSERT_EQ(transform_request->getPosition(), position);
}

TEST_F(StateTest, BlastBotTest) {
    auto bots = state->getBots();
    auto bot = bots[0][0];

    // Checking if the bot's final destination is set after calling blastBot
    ASSERT_EQ(bot->getFinalDestination(), DoubleVec2D::null);
    state->blastBot(bot->getActorId(), DoubleVec2D(3, 2));
    ASSERT_EQ(bot->getFinalDestination(), DoubleVec2D(3, 2));
}

TEST_F(StateTest, BlastTowerTest) {
    auto towers = state->getTowers();
    auto tower = towers[0][0];

    // Checking if the tower's blasting property is set after calling blastTower
    ASSERT_EQ(tower->isBlasting(), false);
    state->blastTower(tower->getActorId());
    ASSERT_EQ(tower->isBlasting(), true);
}

TEST_F(StateTest, RemoveDeadActors) {
    // Killing PLAYER1 bot 1 and PLAYER2 tower1
    auto bots = state->getBots();
    auto towers = state->getTowers();

    auto bot = bots[0][0];
    bot->setHp(0);
    ASSERT_EQ(bots[0].size(), 1);

    auto tower = towers[1][0];
    tower->setHp(0);
    ASSERT_EQ(bots[1].size(), 1);

    // Removing dead actors from state
    state->removeDeadActors();

    bots = state->getBots();
    towers = state->getTowers();

    ASSERT_EQ(bots[0].size(), 0);
    ASSERT_EQ(towers[1].size(), 0);
}

TEST_F(StateTest, SpawnNewBots) {
    // Checking initial number of bots that both players have and checking if
    // the number of bot increases by bot frequency
    auto bots = state->getBots();

    ASSERT_EQ(bots[0].size(), 1);
    ASSERT_EQ(bots[1].size(), 1);

    state->spawnNewBots();

    bots = state->getBots();

    ASSERT_EQ(bots[0].size(), 2);
    ASSERT_EQ(bots[1].size(), 2);
}

TEST_F(StateTest, DamageEnemyActorsTest) {
    // Making all the bots move to the center and making one bot blast
    auto bots = state->getBots();
    auto towers = state->getTowers();

    auto bot = bots[0][0];
    auto bot_position = DoubleVec2D(2, 2);
    auto impact_radius = bot->getBlastRange();
    auto damage_points = bot->getBlastDamage();

    bot->setPosition(bot_position);

    towers[0][0]->setPosition(DoubleVec2D(3, 3));
    bots[1][0]->setPosition(DoubleVec2D(2, 3));
    towers[1][0]->setPosition(DoubleVec2D(1, 1));

    // Making the player1 blast and checking the damage_incurred to the PLAYER2
    // bot and PLAYER2 tower
    state->damageEnemyActors(PlayerId::PLAYER1, bot->getActorId(),
                             bot->getPosition());

    // Checking the damage incurred of PLAYER1 and PLAYER2
    bots = state->getBots();
    towers = state->getTowers();

    // Calculating the damage that tower 1 should have incurred
    double_t bot_distance = bot_position.distance(bots[1][0]->getPosition());
    double_t tower_distance =
        bot_position.distance(towers[1][0]->getPosition());

    double_t bot_remaining_distance = impact_radius - bot_distance;
    double_t tower_remaining_distance = impact_radius - tower_distance;

    double_t normalized_bot_distance = bot_remaining_distance / impact_radius;
    double_t normalized_tower_distance =
        tower_remaining_distance / impact_radius;

    uint64_t damage_incurred_bot = normalized_bot_distance * damage_points;
    uint64_t damage_incurred_tower = normalized_tower_distance * damage_points;

    ASSERT_EQ(towers[0][0]->getDamageIncurred(), 0);
    ASSERT_EQ(bots[1][0]->getDamageIncurred(), damage_incurred_bot);
    ASSERT_EQ(towers[1][0]->getDamageIncurred(), damage_incurred_tower);
}

TEST_F(StateTest, GetAffectedActorsTest) {
    // Moving PLAYER1 bot to center and making the PLAYER2 tower in it's blast
    // range but not the PLAYER2 bot
    auto bots = state->getBots();
    auto towers = state->getTowers();

    auto bot = bots[0][0];
    auto bot_position = DoubleVec2D(2.5, 2.5);
    bot->setPosition(bot_position);
    bot->setBlastRange(2);

    // Moving the PLAYER2 tower within the range of the bot and the PLAYER2 bot
    // outside the blast range
    towers[1][0]->setPosition(DoubleVec2D(3.5, 3.5));
    bots[1][0]->setPosition(DoubleVec2D(4, 4));

    auto affected_actors = state->getAffectedActors(
        bot->getPlayerId(), bot_position, bot->getBlastRange());
    ASSERT_EQ(affected_actors.size(), 1);
    auto affected_actor = affected_actors[0];
    ASSERT_EQ(affected_actor->getActorId(), towers[1][0]->getActorId());
}

TEST_F(StateTest, RejectTransformRequestTest){
    // Moving bots into one square and asking both bots to transform
    auto bots = state->getBots();
    auto bot1 = bots[0][0];
    auto bot2 = bots[1][0];

    bot1->setPosition(DoubleVec2D(2.5, 2.3));
    bot2->setPosition(DoubleVec2D(2.2, 2.7));

    state->transformBot(bot1->getPlayerId(), bot1->getActorId(), bot1->getPosition());
    state->transformBot(bot2->getPlayerId(), bot2->getActorId(), bot2->getPosition());

    // Calling handleTransformRequests and checking to assert that no tower is built
    state->handleTransformRequests();

    bots = state->getBots();
    auto towers = state->getTowers();

    // Checking to see that the bots haven't been killed 
    ASSERT_EQ(bots[0].size(), 1);
    ASSERT_EQ(bots[1].size(), 1);

    // Checking to see that no towers are added
    ASSERT_EQ(towers[0].size(), 1);
    ASSERT_EQ(towers[1].size(), 1);

    // Making both the bots stand on the same offset but only one bot tries to transform
    state->transformBot(bot1->getPlayerId(), bot1->getActorId(), bot1->getPosition());

    // Calling handleTransformRequests and checking to assert that no tower is built
    state->handleTransformRequests();

    bots = state->getBots();
    towers = state->getTowers();

    // Checking to see that the bot isn't killed and no tower is added to the bot
    ASSERT_EQ(bots[0].size(), 1);
    ASSERT_EQ(towers[0].size(), 1);
}

TEST_F(StateTest, AcknowledgeTransformRequest){
    // Making PLAYER1 bot transform
    auto bots = state->getBots();
    auto bot = bots[0][0];
    
    state->transformBot(bot->getPlayerId(), bot->getActorId(), bot->getPosition());

    // Calling handle transform requests to see whether a tower is built
    state->handleTransformRequests();

    bots = state->getBots();
    auto towers = state->getTowers();

    ASSERT_EQ(bots[0][0]->getState(), BotStateName::DEAD);
    ASSERT_EQ(bots[1].size(), 1);

    ASSERT_EQ(towers[0].size(), 2);
    ASSERT_EQ(towers[1].size(), 1);
}
