#include "logger/logger.h"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "It works? It works!!!" << std::endl;

  logger::Logger log = logger::Logger();
  log.LogState();
  return 0;
}
