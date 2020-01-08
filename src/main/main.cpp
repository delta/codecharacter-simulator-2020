#include "logger/logger.h"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "It works? It works!!!" << std::endl;

  // TODO: remove; just for debugging
  logger::Logger log = logger::Logger();
  log.LogState();
  log.LogFinalGameParams(state::PlayerId::PLAYER1);
  log.LogError(state::PlayerId::PLAYER1, logger::ErrorType::POSITION_OCCUPIED,
               "ramdom error message");
  log.LogInstructionCount(state::PlayerId::PLAYER1, 5);
  return 0;
}
