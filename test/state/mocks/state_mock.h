#pragma once

#include "state/interfaces/i_command_taker.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace std;
using namespace state;

class StateMock : public ICommandTaker {
  public:
    MOCK_METHOD2(moveBot, void(ActorId bot_id, DoubleVec2D position));
    MOCK_METHOD2(transformBot, void(ActorId bot_id, DoubleVec2D position));
    MOCK_METHOD2(blast, void(ActorId bot_id, DoubleVec2D position));
    MOCK_METHOD1(blast, void(ActorId bot_id));
    MOCK_CONST_METHOD0(getScores, array<uint64_t, 2>());
    MOCK_METHOD1(isGameOver, bool(PlayerId &winner));
    MOCK_METHOD0(getBots, array<vector<state::Bot *>, 2>());
    MOCK_METHOD0(getTowers, array<vector<state::Tower *>, 2>());
    MOCK_METHOD0(getTransformRequests, array<vector<TransformRequest *>, 2>());
    MOCK_CONST_METHOD0(getMap, Map *());
    MOCK_CONST_METHOD0(getScoreManager, ScoreManager *());
    MOCK_CONST_METHOD0(getPathPlanner, PathPlanner *());
    MOCK_METHOD0(update, void());
    MOCK_METHOD0(lateUpdate, void());
    MOCK_METHOD0(removeDeadActors, void());
};
