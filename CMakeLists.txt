cmake_minimum_required (VERSION 3.21)
set(PROJECT caph)
project (caph
  VERSION 0.0.1
  DESCRIPTION "Caph lib"
  HOMEPAGE_URL https://github.com/all3n/caph
  LANGUAGES C
)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 11)
option(BUILD_STATIC "Build STATIC library" OFF)
option(ENABLE_TESTS "ENABLE TESTS" ON)

include(cmake/thirdparty.cmake)
# 根据构建类型设置编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
endif()
# tools
add_executable(compile_res tools/res_compile.c src/ch_zlib.c src/ch_path.c src/ch_string.c src/ch_logging.c)


file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*tests.c")
set(TEST_COMMON tests/tests_common.c)
if (DEBUG)
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach (_variableName ${_variableNames})
      message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif()


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/public)
execute_process(
  COMMAND ${CMAKE_BINARY_DIR}/compile_res -i ${CMAKE_CURRENT_SOURCE_DIR}/public/dist -o ${CMAKE_BINARY_DIR}/public/ch_res_public.h
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
)
include_directories(src)
include_directories(${CMAKE_BINARY_DIR}/public)

if(BUILD_STATIC)
    add_library(${PROJECT} STATIC ${SOURCES})
    set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE C)
    set_target_properties(${PROJECT} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/static)
else()
    add_library(${PROJECT} SHARED ${SOURCES})
    set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE C)
    set_target_properties(${PROJECT} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/shared)
endif()
# for WIN32 export
target_compile_definitions(${PROJECT} PRIVATE CH_COMPILE_LIBRARY)
if(APPLE)
  include_directories(/opt/homebrew/include)
  link_directories(/opt/homebrew/lib)
endif()
target_link_libraries(${PROJECT} PRIVATE json-c curl microhttpd)



if (ENABLE_TESTS)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/tests/data ${CMAKE_BINARY_DIR}/data
    RESULT_VARIABLE symlink_result
  )
  if(NOT symlink_result EQUAL 0)
      message(FATAL_ERROR "Failed to create symlink for data directory")
  endif()

  enable_testing()

  foreach(TEST_SOURCE ${TEST_SOURCES})
      get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
      add_executable(${TEST_NAME} ${TEST_SOURCE} ${TEST_COMMON})
      target_link_libraries(${TEST_NAME} ${PROJECT} unity)
      add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
  endforeach(TEST_SOURCE)
endif()


install(DIRECTORY src/ DESTINATION include)
install(TARGETS ${PROJECT}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
