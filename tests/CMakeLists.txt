add_executable(tests 
    src/modules/ZellijModuleTests.cpp

    src/util/JSONCParserTests.cpp
    src/util/ParseTests.cpp

    src/Main.cpp
)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.11.0
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests Catch2::Catch2)
target_link_libraries(tests umbrasrc)
target_include_directories(tests PRIVATE ./src/)

# vim:ft=cmake
