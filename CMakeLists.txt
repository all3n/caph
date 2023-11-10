cmake_minimum_required (VERSION 3.21)
set(PROJECT caph)
project (caph
  VERSION 0.0.1
  DESCRIPTION "Caph lib"
  HOMEPAGE_URL https://github.com/all3n/caph
  LANGUAGES C
)

option(CMAKE_BUILD_TYPE "Build Type" Debug)
option(BUILD_STATIC "Build STATIC library" OFF)
option(ENABLE_TESTS "ENABLE TESTS" OFF)
option(ENABLE_WEB "ENABLE WEB" OFF)
option(ENABLE_CROSS_WIN "CROSS WIN USE MINGW" OFF)
set(CROSS_TOOLCHAIN toolchain-x86_64)


# flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# files
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
set(TEST_COMMON tests/tests_common.c)
# file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
set(SOURCES 
  src/ch_curl_utils.c src/ch_md5.c        src/ch_path.c       src/ch_string.c     src/ch_zlib.c
  src/ch_logging.c    src/ch_opts.c       src/ch_sockets.c    src/hashmap.c       src/qrcodegen.c
  src/ch_hash_map.c   src/ch_murmurhash.c
)

# file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*tests.c")
set(TEST_SOURCES
  tests/ch_curl_tests.c   tests/ch_opts_tests.c   
  tests/ch_list_tests.c   tests/ch_path_tests.c
  tests/ch_md5_tests.c    tests/ch_string_tests.c
  tests/qrcode_tests.c    tests/ch_hash_tests.c
  tests/ch_murmurhash_tests.c
)



if(APPLE)
  include_directories(/opt/homebrew/include)
  link_directories(/opt/homebrew/lib)
  if(ENABLE_CROSS_WIN)
    message(STATUS "CROSS WIN USE MINGW")
    set(MINGW_BASE /opt/homebrew/opt/mingw-w64)
    include_directories(${MINGW_BASE}/${CROSS_TOOLCHAIN}/include)
    link_directories(${MINGW_BASE}/${CROSS_TOOLCHAIN}/lib)
    set(CMAKE_C_COMPILER "${MINGW_BASE}/${CROSS_TOOLCHAIN}/bin/x86_64-w64-mingw32-gcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=x86-64")
  endif()
endif()



if(ENABLE_TESTS)
  include(cmake/thirdparty_test.cmake)
endif()
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

if (ENABLE_WEB)
    # tools
    list(APPEND SOURCES src/ch_web.c)
    list(APPEND TEST_SOURCES tests/ch_web_tests.c)
    add_executable(compile_res tools/res_compile.c src/ch_zlib.c src/ch_path.c src/ch_string.c src/ch_logging.c)
endif()





if (DEBUG)
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach (_variableName ${_variableNames})
      message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif()

include_directories(src)

if (ENABLE_WEB)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/public)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/public/ch_res_public.h
        COMMAND ${CMAKE_BINARY_DIR}/compile_res -i ${CMAKE_CURRENT_SOURCE_DIR}/public/dist -o ${CMAKE_BINARY_DIR}/public/ch_res_public.h
        DEPENDS ${CMAKE_BINARY_DIR}/compile_res
        COMMENT "compile res"
    )
    add_custom_target(RES_HEAD DEPENDS ${CMAKE_BINARY_DIR}/public/ch_res_public.h)
    include_directories(${CMAKE_BINARY_DIR}/public)
endif()

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
target_link_libraries(${PROJECT} PUBLIC json-c curl)

if (ENABLE_WEB)
  add_dependencies(${PROJECT} RES_HEAD)
  target_link_libraries(${PROJECT} PUBLIC microhttpd)
endif()

if (ENABLE_TESTS)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/tests/data ${CMAKE_BINARY_DIR}/data
    RESULT_VARIABLE symlink_result
  )
  if(NOT symlink_result EQUAL 0)
      message(FATAL_ERROR "Failed to create symlink for data directory")
  endif()


  foreach(TEST_SOURCE ${TEST_SOURCES})
      get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
      add_executable(${TEST_NAME} ${TEST_SOURCE} ${TEST_COMMON})
      target_link_libraries(${TEST_NAME} ${PROJECT} unity)
  endforeach(TEST_SOURCE)
endif()


install(FILES ${HEADER_FILES} DESTINATION include)
install(TARGETS ${PROJECT}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
