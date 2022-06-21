cmake_minimum_required(VERSION 3.17)
project(CN_CA4)

set(CMAKE_CXX_STANDARD 11)

add_executable(Router Source/Router/main.cpp)
add_executable(Client Source/Client/main.cpp)