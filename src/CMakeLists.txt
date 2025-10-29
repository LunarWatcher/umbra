add_library(umbrasrc STATIC 
    umbra/ModuleLoader.cpp

    umbra/except/Exception.cpp

    umbra/modules/DevenvModule.cpp
    umbra/modules/Module.cpp
    umbra/modules/ZellijModule.cpp

    umbra/util/FilesystemExt.cpp
    umbra/util/Parse.cpp

    umbra/wranglers/Shell.cpp
)
add_executable(
    umbra umbra/Main.cpp
)
target_link_libraries(
    umbrasrc
PUBLIC
    stc
    CLI11::CLI11
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)
target_link_libraries(umbra PUBLIC umbrasrc)

target_include_directories(umbrasrc PUBLIC ./)

# Largely for development use
add_custom_target(resources
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/share
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/share/umbra

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shell ${CMAKE_BINARY_DIR}/share/umbra
    COMMENT "Copying resources"
)
add_dependencies(umbrasrc resources)


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
