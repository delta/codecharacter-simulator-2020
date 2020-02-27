#pragma once

#include "logger/interfaces/i_logger.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;
using namespace state;
using namespace logger;

class LoggerMock : public ILogger {
  public:
    MOCK_METHOD0(logState, void());
    MOCK_METHOD2(logInstructionCount, void(PlayerId player_id, size_t count));
    MOCK_METHOD3(logError, void(state::PlayerId player_id, ErrorType error_type,
                                std::string message));
    MOCK_METHOD2(logFinalGameParams,
                 void(state::PlayerId player_id,
                      std::array<uint64_t, 2> final_scores));
    MOCK_METHOD1(writeGame, void(std::ostream &write_stream));
};
