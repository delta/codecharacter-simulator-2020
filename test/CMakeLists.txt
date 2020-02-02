cmake_minimum_required(VERSION 3.15.0)
project(tests)

set(SOURCE_FILES test_main.cpp physics/vector_test.cpp state/map_test.cpp
                 state/graph_test.cpp state/path_graph_test.cpp state/tower_test.cpp state/path_planner_test.cpp)

add_executable(tests ${SOURCE_FILES})

target_link_libraries(tests physics state gtest gmock)

install(TARGETS tests DESTINATION bin)
