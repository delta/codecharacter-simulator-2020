# Creates target to perform cmake-format run, requires cmake-format

# Find cmake format
find_package(CmakeFormat)

if(CMAKE_FORMAT_FOUND)
  message("** Found cmake-format")
  message("** cmake-format executable location: ${CMAKE_FORMAT_EXECUTABLE}")
  message("** cmake-format version: ${CMAKE_FORMAT_VERSION}")
else()
  message(SEND_ERROR "cmake-format executable not found")
endif()

file(GLOB_RECURSE ALL_CMAKE_FILES *.cmake *CMakeLists.txt)

list(FILTER ALL_CMAKE_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/build*)
list(FILTER ALL_CMAKE_FILES EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/ext*)
list(FILTER ALL_CMAKE_FILES EXCLUDE REGEX ${CMAKE_INSTALL_PREFIX}/*)

add_custom_target(cmakeformat ALL COMMAND ${CMAKE_FORMAT_EXECUTABLE} -i
                                          ${ALL_CMAKE_FILES})
