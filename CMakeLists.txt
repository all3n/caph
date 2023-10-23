cmake_minimum_required (VERSION 3.21)
set(PROJECT caph)
project (caph
  VERSION 0.0.1
  DESCRIPTION "Caph lib"
  HOMEPAGE_URL https://github.com/all3n/caph
  LANGUAGES C
)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")
if (DEBUG)
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach (_variableName ${_variableNames})
      message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif()




include_directories(src)

add_library(${PROJECT} SHARED ${SOURCES})


foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} ${PROJECT})
endforeach(TEST_SOURCE)
