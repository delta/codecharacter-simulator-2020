# Utility to find cmake-format

string(REPLACE ":" ";" __PATH $ENV{PATH})

foreach(p ${__PATH})
  file(GLOB cmke ${p}/cmake-format ${p}/cmake-format-[0-9]*)
  if(cmke)
    set(CMAKE_FORMAT_EXECUTABLE ${cmke})
    set(CMAKE_FORMAT_FOUND ON)
    execute_process(COMMAND ${CMAKE_FORMAT_EXECUTABLE} --version
                    OUTPUT_VARIABLE cmake_out)
    set(CMAKE_FORMAT_VERSION ${cmake_out})
    break()
  else()
    set(CMAKE_FORMAT_FOUND OFF)
  endif()
endforeach(p ${__PATH})
