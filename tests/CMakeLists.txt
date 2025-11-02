add_executable(umbra_tests 
    src/modules/DevenvModuleTests.cpp
    src/modules/ZellijModuleTests.cpp

    src/modules/devenv/ConfigTests.cpp

    src/util/JSONCParserTests.cpp
    src/util/ParseTests.cpp

    # Meta tests
    src/testutil/selftest/EnvTests.cpp

    src/Main.cpp
)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.11.0
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(umbra_tests Catch2::Catch2)
target_link_libraries(umbra_tests umbrasrc)
target_include_directories(umbra_tests PRIVATE ./src/)

# vim:ft=cmake
