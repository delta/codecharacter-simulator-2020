cmake_minimum_required(VERSION 3.15.0)
project(state)

set(SOURCE_FILES
    src/state.cpp
    src/map/map.cpp
    src/path_planner/graph/graph.cpp
    src/actor/actor.cpp
    src/actor/unit.cpp
    src/path_planner/path_graph_helper.cpp
    src/path_planner/path_graph.cpp)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/state/state_export.h)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.68.0 REQUIRED)

add_library(state SHARED ${SOURCE_FILES})
target_link_libraries(state physics constants)

generate_export_header(state EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(
  state
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
         $<BUILD_INTERFACE:${EXPORTS_DIR}>
         $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}> $<INSTALL_INTERFACE:include>)

install(
  TARGETS state
  EXPORT state_config
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(EXPORT state_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/state)
