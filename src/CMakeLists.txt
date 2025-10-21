add_library(umbrasrc STATIC 
    umbra/ModuleLoader.cpp

    umbra/except/Exception.cpp

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

# vim:ft=cmake
