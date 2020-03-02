#include "state/state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;
using namespace state;

const auto L = state::TerrainType::LAND;
const auto W = state::TerrainType::WATER;
const auto F = state::TerrainType::FLAG;

class StateTest : public Test {
  public:
    unique_ptr<State> state;
    size_t map_size;
    int64_t bot_max_hp, tower_max_hp;

    StateTest() {
        map_size = 5;

        auto score_manager = make_unique<ScoreManager>();

        // clang-format off
        auto test_map = vector<vector<TerrainType>>{
            {L, L, L, L, W}, 
            {L, L, L, L, L},
            {L, L, L, L, L},
            {L, L, L, F, L},
            {W, L, L, L, L},
        };
        // clang-format on

        auto map = make_unique<Map>(test_map, map_size);
        auto path_planner = make_unique<PathPlanner>(map.get());

        // Creating bots and towers
        int64_t bot_hp = 100, speed = 5, bot_blast_range = 3,
                bot_damage_points = 30;
        bot_max_hp = 100;

        auto model_bot =
            Bot(1, PlayerId::PLAYER1, bot_hp, bot_max_hp, DoubleVec2D(1, 1),
                speed, bot_blast_range, bot_damage_points, score_manager.get(),
                path_planner.get(), BlastCallback{}, ConstructTowerCallback{});

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
        tower_max_hp = 300;

        auto model_tower =
            Tower(PlayerId::PLAYER1, tower_hp, tower_max_hp, DoubleVec2D(0, 0),
                  tower_damage_points, tower_blast_range, score_manager.get(),
                  BlastCallback{});

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
                                   move(state_towers), move(model_bot),
                                   move(model_tower));
    }
};

TEST_F(StateTest, GetActors) {
    auto bots = state->getBots();
    auto towers = state->getTowers();

    EXPECT_EQ(bots[0].size(), 1);
    EXPECT_EQ(bots[1].size(), 1);
    EXPECT_EQ(towers[0].size(), 1);
    EXPECT_EQ(towers[1].size(), 1);
}

TEST_F(StateTest, GetMap) {
    auto map = state->getMap();
    EXPECT_EQ(!map, false);

    size_t map_size = map->getSize();
    EXPECT_EQ(map_size, 5);
    size_t p1 = 0, p2 = 3;
    EXPECT_EQ(map->getTerrainType(p1, p1), TerrainType::LAND);
    EXPECT_EQ(map->getTerrainType(p2, p2), TerrainType::FLAG);
}

TEST_F(StateTest, MoveBotTest) {
    auto bots = state->getBots();
    auto bot = bots[0][0];
    EXPECT_EQ(bot->isDestinationSet(), false);

    // Calling move bot in state and checking if bots destination is set
    state->moveBot(1, DoubleVec2D(2, 2));

    // Getting PLAYER1's first bot and checking if his destination is set
    DoubleVec2D bot_destination = bot->getDestination();
    EXPECT_EQ(bot->isDestinationSet(), true);
    EXPECT_EQ(bot_destination, DoubleVec2D(2, 2));
}

TEST_F(StateTest, CreateTowerTest) {
    // Transforming the first bot of PLAYER2 into a tower
    auto bots = state->getBots();
    auto bot = bots[0][0];
    auto score_manager = state->getScoreManager();

    int64_t bot_id = bot->getActorId();
    int64_t hp = 50;
    bot->setHp(50);

    EXPECT_EQ(bots[0].size(), 1);
    EXPECT_EQ(bots[1].size(), 1);

    bot->setDestination(DoubleVec2D(3.5, 3.5));
    bot->update();
    bot->lateUpdate();

    // Checking initial number of bots on flags
    ASSERT_EQ(bot->getPosition(), DoubleVec2D(3.5, 3.5));
    auto bot_counts = score_manager->getBotCounts();
    EXPECT_EQ(bot_counts[0], 1);

    // Calling create tower on this bot
    state->produceTower(bot);

    bots = state->getBots();
    auto towers = state->getTowers();

    // Checking to see if the bot was removed from state and if a tower was
    // added to state
    EXPECT_EQ(bots[0].size(), 1);
    EXPECT_EQ(towers[0].size(), 2);

    // Checking to see if the tower has the same id as the bot
    auto tower = towers[0][1];
    EXPECT_EQ(tower->getActorId(), bot_id);

    // Check if the tower HP has been scaled correctly
    double hp_ratio = double(tower_max_hp) / double(bot_max_hp);

    int64_t tower_hp = hp * hp_ratio;
    EXPECT_EQ(tower->getHp(), tower_hp);

    // Checking score_manager counts because the bot was on a FLAG
    bot_counts = score_manager->getBotCounts();
    auto tower_counts = score_manager->getTowerCounts();
    EXPECT_EQ(tower_counts[0], 1);
    EXPECT_EQ(bot_counts[0], 0);
}

TEST_F(StateTest, TransformRequestTest) {
    auto bots = state->getBots();
    auto bot = bots[0][0];
    auto actor_id = bot->getActorId();
    auto player_id = bot->getPlayerId();
    auto position = bot->getPosition();
    using namespace std::placeholders;
    auto construct_tower_callback =
        std::bind(&State::constructTowerCallback, state.get(), _1);
    bot->setConstructTowerCallback(construct_tower_callback);

    auto transform_requests = state->getTransformRequests();
    EXPECT_EQ(transform_requests[0].size(), 0);
    EXPECT_EQ(transform_requests[1].size(), 0);

    state->transformBot(actor_id, position);
    bot->update();
    bot->lateUpdate();

    transform_requests = state->getTransformRequests();
    EXPECT_EQ(transform_requests[0].size(), 1);

    auto transform_request = transform_requests[0][0];
    EXPECT_EQ(transform_request->getPlayerId(), player_id);
    EXPECT_EQ(transform_request->getBotId(), actor_id);
    EXPECT_EQ(transform_request->getPosition(), position);
}

TEST_F(StateTest, BlastBotTest) {
    auto bots = state->getBots();
    auto bot = bots[0][0];
    auto blast_bot_callback =
        std::bind(&State::damageEnemyActors, state.get(), placeholders::_1,
                  placeholders::_2, placeholders::_3);

    // Checking if the bot's final destination is set after calling blastBot
    bot->setBlastCallback(blast_bot_callback);
    EXPECT_EQ(bot->getFinalDestination(), DoubleVec2D::null);
    state->blast(bot->getActorId(), DoubleVec2D(4, 1));
    EXPECT_EQ(bot->getFinalDestination(), DoubleVec2D(4, 1));

    while (bot->getState() != BotStateName::DEAD) {
        bot->update();
        bot->lateUpdate();
    }

    EXPECT_EQ(bot->getHp(), 0);
    EXPECT_EQ(bot->getState(), BotStateName::DEAD);
}

TEST_F(StateTest, BlastTowerTest) {
    auto towers = state->getTowers();
    auto tower = towers[0][0];

    // Assigning the tower's BlastCallback
    auto blast_tower_callback =
        std::bind(&State::damageEnemyActors, state.get(), placeholders::_1,
                  placeholders::_2, placeholders::_3);

    // Checking if the tower's blasting property is set after calling blast
    tower->setBlastCallback(blast_tower_callback);
    EXPECT_EQ(tower->isBlasting(), false);
    state->blast(tower->getActorId());
    EXPECT_EQ(tower->isBlasting(), true);

    // Updating state and removing dead actors to check if the tower is removed
    // from state
    state->update();
    state->removeDeadActors();

    towers = state->getTowers();
    EXPECT_EQ(towers[0].size(), 0);
}

TEST_F(StateTest, RemoveDeadActors) {
    // Killing PLAYER1 bot 1 and PLAYER2 tower1
    auto bots = state->getBots();
    auto towers = state->getTowers();

    EXPECT_EQ(bots[0].size(), 1);

    auto bot = bots[0][0];
    bot->setHp(0);
    bot->update();

    EXPECT_EQ(bot->getState(), BotStateName::DEAD);

    EXPECT_EQ(bots[1].size(), 1);
    auto tower = towers[1][0];
    tower->setHp(0);
    tower->update();

    EXPECT_EQ(tower->getState(), TowerStateName::DEAD);

    // Removing dead actors from state
    state->removeDeadActors();

    bots = state->getBots();
    towers = state->getTowers();

    EXPECT_EQ(bots[0].size(), 0);
    EXPECT_EQ(towers[1].size(), 0);
}

TEST_F(StateTest, SpawnNewBots) {
    // Checking initial number of bots that both players have and checking if
    // the number of bot increases by bot frequency
    auto bots = state->getBots();

    EXPECT_EQ(bots[0].size(), 1);
    EXPECT_EQ(bots[1].size(), 1);

    state->spawnNewBots();

    bots = state->getBots();

    EXPECT_EQ(bots[0].size(), 2);
    EXPECT_EQ(bots[1].size(), 2);
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

    EXPECT_EQ(towers[0][0]->getDamageIncurred(), 0);
    EXPECT_EQ(bots[1][0]->getDamageIncurred(), damage_incurred_bot);
    EXPECT_EQ(towers[1][0]->getDamageIncurred(), damage_incurred_tower);
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
    EXPECT_EQ(affected_actors.size(), 1);
    auto affected_actor = affected_actors[0];
    EXPECT_EQ(affected_actor->getActorId(), towers[1][0]->getActorId());
}

TEST_F(StateTest, RejectTransformRequestTest) {
    // Moving bots into one square and asking both bots to transform
    auto bots = state->getBots();
    auto bot1 = bots[0][0];
    auto bot2 = bots[1][0];

    bot1->setPosition(DoubleVec2D(2.5, 2.3));
    bot2->setPosition(DoubleVec2D(2.2, 2.7));

    state->transformBot(bot1->getActorId(), bot1->getPosition());
    state->transformBot(bot2->getActorId(), bot2->getPosition());

    // Calling handleTransformRequests and checking to assert that no tower is
    // built
    state->handleTransformRequests();

    bots = state->getBots();
    auto towers = state->getTowers();

    // Checking to see that the bots haven't been killed
    EXPECT_EQ(bots[0].size(), 1);
    EXPECT_EQ(bots[1].size(), 1);

    // Checking to see that no towers are added
    EXPECT_EQ(towers[0].size(), 1);
    EXPECT_EQ(towers[1].size(), 1);

    // Making both the bots stand on the same offset but only one bot tries to
    // transform
    state->transformBot(bot1->getActorId(), bot1->getPosition());

    // Calling handleTransformRequests and checking to assert that no tower is
    // built
    state->handleTransformRequests();

    bots = state->getBots();
    towers = state->getTowers();

    // Checking to see that the bot isn't killed and no tower is added to the
    // bot
    EXPECT_EQ(bots[0].size(), 1);
    EXPECT_EQ(towers[0].size(), 1);
}

TEST_F(StateTest, AcknowledgeTransformRequest) {
    // Making PLAYER1 bot transform
    auto bots = state->getBots();
    auto bot = bots[0][0];
    using namespace std::placeholders;
    auto construct_tower_callback =
        std::bind(&State::constructTowerCallback, state.get(), _1);
    bot->setConstructTowerCallback(construct_tower_callback);

    state->transformBot(bot->getActorId(), bot->getPosition());
    bot->update();
    bot->lateUpdate();

    // Calling handle transform requests to see whether a tower is built
    state->handleTransformRequests();

    bots = state->getBots();
    auto towers = state->getTowers();

    EXPECT_EQ(bots[0][0]->getState(), BotStateName::DEAD);
    EXPECT_EQ(bots[1].size(), 1);

    EXPECT_EQ(towers[0].size(), 2);
    EXPECT_EQ(towers[1].size(), 1);
}
