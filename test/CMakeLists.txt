cmake_minimum_required(VERSION 3.15.0)
project(tests)

set(SOURCE_FILES
    test_main.cpp
    state/map_test.cpp
    state/graph_test.cpp
    state/tower_test.cpp
    state/bot_test.cpp
    physics/vector_test.cpp
    state/path_graph_test.cpp
    state/path_planner_test.cpp
    state/command_giver_test.cpp
    state/score_manager_test.cpp
    state/state_syncer_test.cpp
    logger/logger_test.cpp)

if(NOT BUILD_PROJECT STREQUAL "all")
  include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
  include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
  include(${CMAKE_INSTALL_PREFIX}/lib/drivers_config.cmake)
  include(${CMAKE_INSTALL_PREFIX}/lib/logger_config.cmake)
endif()

include_directories(.)

add_executable(tests ${SOURCE_FILES})

target_link_libraries(
  tests
  physics
  constants
  state
  logger
  player_wrapper
  gtest
  gmock)

install(TARGETS tests DESTINATION bin)
