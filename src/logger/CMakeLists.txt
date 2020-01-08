cmake_minimum_required(VERSION 3.15.0)
project(logger)

set(SOURCE_FILES src/logger.cpp)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/logger/logger_export.h)

add_library(logger SHARED ${SOURCE_FILES})
target_link_libraries(logger state physics)

generate_export_header(logger EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(
  logger PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
                $<BUILD_INTERFACE:${EXPORTS_DIR}> $<INSTALL_INTERFACE:include>)

install(
  TARGETS logger
  EXPORT logger_config
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(EXPORT logger_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/logger)
