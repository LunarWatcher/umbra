add_library(umbrasrc STATIC 
    umbra/ModuleLoader.cpp

    umbra/except/Exception.cpp

    umbra/modules/Module.cpp
    umbra/modules/ZellijModule.cpp

    umbra/util/FilesystemExt.cpp
    umbra/util/Parse.cpp
)
add_executable(
    umbra umbra/Main.cpp
)
target_link_libraries(umbrasrc PUBLIC stc CLI11::CLI11)
target_link_libraries(umbra PUBLIC umbrasrc)

target_include_directories(umbrasrc PUBLIC ./)

# Linting {{{
if (UMBRA_LINT)
    find_program(
        CLANGTIDY
        NAMES "clang-tidy" "clang-tidy.exe"
        REQUIRED
    )

    set (CLANG_TIDY_COMMAND "${CLANGTIDY}" "--config-file=${CMAKE_CURRENT_SOURCE_DIR}/../.clang-tidy")
    if (UMBRA_LINT_WARNINGS_ARE_ERRORS)
        list (APPEND CLANG_TIDY_COMMAND  -warnings-as-errors=*)
    endif()

    if (WIN32)
        list (APPEND CLANG_TIDY_COMMAND --extra-arg=/EHsc)
    endif()

    message(STATUS "Using CLANG_TIDY_COMMAND=${CLANG_TIDY_COMMAND}")
    set_target_properties(umbra PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    set_target_properties(umbrasrc PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()
# }}}
# Installation {{{
install(
    TARGETS umbra
    DESTINATION bin
)
# }}}
# vim:ft=cmake
