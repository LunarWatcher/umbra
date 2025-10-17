#add_library(umbrasrc STATIC 
    # Non-executable files go here
#)
add_executable(umbra umbra/Main.cpp)

target_include_directories(umbra PUBLIC ./)

# vim:ft=cmake
