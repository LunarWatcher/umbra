cmake_minimum_required(VERSION 3.10)
project(umbra)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.5.0
)
FetchContent_Declare(stc
    GIT_REPOSITORY https://github.com/LunarWatcher/stc
)
FetchContent_MakeAvailable(cli11 stc)

add_subdirectory(src)
add_subdirectory(tests)

add_custom_target(run
    COMMAND umbra
    DEPENDS umbra
    COMMENT "Run umbra")
add_custom_target(test
    COMMAND tests
    DEPENDS tests
    COMMENT "Test umbra")
# vim:ft=cmake
