cmake_minimum_required(VERSION 3.15.0)
project(drivers)

set(SOURCE_FILES
    src/shared_memory_utils/shared_memory_main.cpp
    src/shared_memory_utils/shared_memory_player.cpp
    src/shared_memory_utils/shared_buffer.cpp
    src/timer.cpp
    src/player_driver.cpp
    src/main_driver.cpp)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/drivers/drivers_export.h)

find_package(Boost 1.68.0)

add_library(drivers STATIC ${SOURCE_FILES})

target_link_libraries(drivers ${CMAKE_THREAD_LIBS_INIT} player_wrapper)

generate_export_header(drivers EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(
  drivers
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
         $<BUILD_INTERFACE:${EXPORTS_DIR}>
         $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}> $<INSTALL_INTERFACE:include>)

install(
  TARGETS drivers
  EXPORT drivers_config
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(EXPORT drivers_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/drivers)
