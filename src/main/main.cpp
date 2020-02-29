#include "constants/constants.h"
#include "drivers/main_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/timer.h"
#include "game/game.h"
#include "logger/logger.h"
#include "physics/vector.hpp"
#include "state/actor/actor.h"
#include "state/actor/bot.h"
#include "state/actor/tower.h"
#include "state/command_giver.h"
#include "state/map/map.h"
#include "state/path_planner/path_planner.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/state_syncer.h"
#include "state/utilities.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>

using namespace std;
using namespace drivers;
using namespace state;
using namespace logger;
using namespace physics;
using namespace Constants::Actor;
using namespace Constants::Map;
using namespace Constants::Simulator;

// The map file contains the terrain layout for initializing the game map
const auto MAP_FILE_NAME = "map.txt";

// The security key file contains a single string, which is prefixed to the
// output scores so that the player cannot directly print a score
const auto KEY_FILE_NAME = "key.txt";

auto shm_names = vector<string>(2);

unique_ptr<Map> buildMap() {
    auto map_elements = vector<vector<TerrainType>>{};

    auto map_file = ifstream(MAP_FILE_NAME, ifstream::in);

    // Compute file size
    map_file.seekg(0, basic_ifstream<char>::end);
    auto map_file_size = map_file.tellg();
    map_file.seekg(0);

    auto map_file_input = vector<char>(map_file_size, '\0');
    map_file.read(&map_file_input[0], map_file_size);

    auto map_row = vector<TerrainType>{};
    for (auto character : map_file_input) {
        switch (character) {
        case 'L':
            map_row.push_back(TerrainType::LAND);
            break;

        case 'W':
            map_row.push_back(TerrainType::WATER);
            break;

        case 'F':
            map_row.push_back(TerrainType::FLAG);
            break;

        case ' ':
            // Ignore all whitespaces
            break;

        case '\n':
            if (map_row.size() != MAP_SIZE) {
                std::cerr << "Bad map file! Match MAP_SIZE " << MAP_SIZE
                          << '\n';
                exit(EXIT_FAILURE);
            }

            map_elements.push_back(map_row);
            map_row.clear();
            break;

        default:
            std::cerr << "Bad map file! Invalid character " << character
                      << '\n';
            exit(EXIT_FAILURE);
            break;
        }
    }
    map_file.close();

    // Ensure that number of rows matches MAP_SIZE
    if (map_elements.size() != MAP_SIZE) {
        std::cerr << "Bad map file! Match MAP_SIZE should be " << MAP_SIZE
                  << '\n';
        exit(EXIT_FAILURE);
    }

    return make_unique<Map>(map_elements, MAP_SIZE);
}

unique_ptr<ScoreManager> buildScoreManager() {
    using namespace Constants::Score;
    return make_unique<ScoreManager>(std::array<uint64_t, 2>{0, 0});
}

unique_ptr<PathPlanner> buildPathPlanner(Map *map) {
    return make_unique<PathPlanner>(map);
}

unique_ptr<Bot> buildBot(PlayerId player_id, PathPlanner *path_planner,
                         ScoreManager *score_manager) {
    return make_unique<Bot>(
        player_id, BOT_MAX_HP, BOT_MAX_HP,
        PLAYER_BASE_POSITIONS[static_cast<int>(player_id)], BOT_SPEED,
        BOT_BLAST_IMPACT_RADIUS, BOT_BLAST_DAMAGE_POINTS, score_manager,
        path_planner, BlastCallback{}, ConstructTowerCallback{});
}

Bot buildModelBot(PathPlanner *path_planner, ScoreManager *score_manager) {
    return Bot(PlayerId::PLAYER1, BOT_MAX_HP, BOT_MAX_HP,
               PLAYER_BASE_POSITIONS[0], BOT_SPEED, BOT_BLAST_IMPACT_RADIUS,
               BOT_BLAST_DAMAGE_POINTS, score_manager, path_planner,
               BlastCallback{}, ConstructTowerCallback{});
}

unique_ptr<Tower> buildTower(PlayerId player_id, ScoreManager *score_manager) {
    return make_unique<Tower>(
        player_id, TOWER_MAX_HP, TOWER_MAX_HP,
        PLAYER_BASE_POSITIONS[static_cast<size_t>(player_id)],
        TOWER_BLAST_DAMAGE_POINTS, TOWER_BLAST_IMPACT_RADIUS, score_manager,
        BlastCallback{});
}

Tower buildModelTower(ScoreManager *score_manager) {
    return Tower(PlayerId::PLAYER1, TOWER_MAX_HP, TOWER_MAX_HP,
                 PLAYER_BASE_POSITIONS[0], TOWER_BLAST_DAMAGE_POINTS,
                 TOWER_BLAST_IMPACT_RADIUS, score_manager, BlastCallback{});
}

unique_ptr<State> buildState() {
    auto map = buildMap();
    auto path_planner = buildPathPlanner(map.get());
    auto score_manager = buildScoreManager();

    auto model_bot = buildModelBot(path_planner.get(), score_manager.get());
    auto model_tower = buildModelTower(score_manager.get());

    auto bots = array<vector<unique_ptr<Bot>>, 2>{};
    auto towers = array<vector<unique_ptr<Tower>>, 2>{};

    // Initialize bots list
    for (int player_id = 0; player_id < 2; ++player_id) {
        auto &player_bots = bots[(int) player_id];
        player_bots.reserve(NUM_BOTS_START);

        for (size_t i = 0; i < NUM_BOTS_START; ++i) {
            player_bots.push_back(buildBot(static_cast<PlayerId>(player_id),
                                           path_planner.get(),
                                           score_manager.get()));
        }
    }

    return make_unique<State>(move(map), move(score_manager),
                              move(path_planner), move(bots), move(towers),
                              move(model_bot), move(model_tower));
}

unique_ptr<MainDriver> buildMainDriver() {
    auto state = buildState();
    auto logger = make_unique<Logger>(
        state.get(), PLAYER_INSTRUCTION_LIMIT_TURN,
        PLAYER_INSTRUCTION_LIMIT_GAME, MAX_BOT_HP, MAX_TOWER_HP);

    auto command_giver = make_unique<CommandGiver>(state.get(), logger.get());
    auto state_syncer = make_unique<StateSyncer>(
        move(state), move(command_giver), logger.get());

    vector<unique_ptr<SharedMemoryMain>> shm_mains;
    for (int i = 0; i < 2; ++i) {
        shm_names[i] = Game::generateRandomString(64) + to_string(i);
        shm_mains.push_back(make_unique<SharedMemoryMain>(
            shm_names[i], false, false, 0, transfer_state::State()));
    }

    return make_unique<MainDriver>(
        move(state_syncer), move(shm_mains), PLAYER_INSTRUCTION_LIMIT_TURN,
        PLAYER_INSTRUCTION_LIMIT_GAME, NUM_TURNS,
        Timer::Interval(GAME_DURATION_MS), move(logger), GAME_LOG_FILE_NAME);
}

string GetKeyFromFile() {
    ifstream key_file(KEY_FILE_NAME, ifstream::in);
    string read_buffer;
    getline(key_file, read_buffer);
    return read_buffer;
}

bool FileExists(const std::string &name) {
    ifstream f(name.c_str());
    return f.good();
}

void WriteToFile(const string &file_name, const string &content) {
    ofstream file(file_name, ofstream::out);
    file << content;
    file.close();
}

int main(int argc, char *argv[]) {
    // Check if map file exists
    if (not FileExists(MAP_FILE_NAME)) {
        cerr << "Error! Could not open map file " << MAP_FILE_NAME << '\n';
        return EXIT_FAILURE;
    }

    // Handle prefix security key
    string prefix_key = "codecharacter";
    if (not FileExists(KEY_FILE_NAME)) {
        cerr << "Warning! Could not open key file. Using default key...\n"
             << "Please add a security key in file " << KEY_FILE_NAME << '\n';
    } else {
        prefix_key = GetKeyFromFile();

        // We've read the key, delete the file for safety
        remove(KEY_FILE_NAME);
    }

    // Build main driver
    auto driver = buildMainDriver();

    // Write the SHM names to file, to be read by the player process
    for (int i = 0; i < 2; ++i) {
        WriteToFile(SHM_FILE_NAMES[i], shm_names[i]);
    }

    // Build game object
    auto game = std::make_unique<Game>(move(driver));

    // Start the game
    cout << "Starting game...\n";
    auto results = game->start();

    // Game has finished
    cout << prefix_key << " " << results << endl;

    return 0;
}
