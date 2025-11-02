cmake_minimum_required(VERSION 3.10)
project(umbra)

set (CMAKE_CXX_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option (UMBRA_LINT "Whether or not to enable clang-tidy checks" OFF)
option (UMBRA_LINT_WARNINGS_ARE_ERRORS "Whether or not to set -warnings-as-errors" OFF)
option (UMBRA_SANITISE "Whether or not to enable -fsanitize=undefined" OFF)
option (UMBRA_COVERAGE "Whether or not to enable test coverage" OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(SHARED OFF)
set(SPDLOG_USE_STD_FORMAT ON CACHE STRING "" FORCE)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wno-c++11-narrowing")

if (UMBRA_SANITISE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

include(FetchContent)
FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.5.0
    EXCLUDE_FROM_ALL
)
FetchContent_Declare(stc
    GIT_REPOSITORY https://github.com/LunarWatcher/stc
)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.16.0
    EXCLUDE_FROM_ALL
)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(
    cli11
    stc
    spdlog
    json
)
# Set up versioning {{{
find_program(GIT git)
if (GIT)
    execute_process(COMMAND ${GIT} describe --tags --always
        OUTPUT_VARIABLE UMBRA_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Detected version: ${UMBRA_VERSION}")
else()
    message(WARNING "Git not found. Defaulting to unknown version. This will not have an effect on the program beyond a missing version")
    set(UMBRA_VERSION "VERSION UNKNOWN. Who doesn't have Git? It's the 2020s. Install it already")
endif()
add_definitions(-DUMBRA_VERSION="${UMBRA_VERSION}")
# }}}

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUMBRA_DEBUG")
endif()

if (UMBRA_COVERAGE)
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" CMAKE_CXX_COMPILER_VERSION_REGEX_MATCH  ${CMAKE_CXX_COMPILER_VERSION})
    set(COMPILER_MAJOR ${CMAKE_MATCH_1})
    find_program(GCOV NAMES gcov-${COMPILER_MAJOR} gcov REQUIRED)
    find_program(LCOV lcov REQUIRED)
    find_program(GENHTML genhtml REQUIRED)

    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
    add_custom_target(gen-coverage
        COMMAND ${LCOV} --capture --output coverage.info --include "${CMAKE_SOURCE_DIR}/src/\\*" --exclude "${CMAKE_BINARY_DIR}/_deps/\\*" --directory . --gcov-tool ${GCOV} --ignore-errors inconsistent
        COMMAND ${GENHTML} coverage.info --output coverage-html --ignore-errors inconsistent
    )
endif()

add_subdirectory(src)
add_subdirectory(tests EXCLUDE_FROM_ALL)

add_custom_target(run
    COMMAND umbra
    DEPENDS umbra
    COMMENT "Run umbra")
add_custom_target(test
    COMMAND tests
    DEPENDS tests
    COMMENT "Test umbra")
# vim:ft=cmake
