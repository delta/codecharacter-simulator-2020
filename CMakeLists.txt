cmake_minimum_required(VERSION 3.15.0)
project(simulator)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wc++2a-extensions")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(GenerateExportHeader)

set(CMAKE_CXX_STANDARD 14)

# linter modules go here
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/linter_config/")

set(BUILD_PROJECT
    "all"
    CACHE STRING "Set the name of the project to build")

# Always enable colors, since Ninja strips them
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-fcolor-diagnostics)
endif()

if(NOT BUILD_PROJECT STREQUAL "no_tests")
  include(${CMAKE_MODULE_PATH}/clang-format.cmake)
  include(${CMAKE_MODULE_PATH}/cmake-format.cmake)
endif()

set(BOOST_ROOT
    ""
    CACHE PATH "Path to Boost libraries")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(src/logger)
add_subdirectory(ext/googletest)
add_subdirectory(src/player_wrapper)
add_subdirectory(src/main)
add_subdirectory(src/physics)
add_subdirectory(src/llvm_pass)
add_subdirectory(src/constants)
add_subdirectory(src/drivers)
add_subdirectory(src/state)
add_subdirectory(src/player_code)
add_subdirectory(test)
