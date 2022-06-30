cmake_minimum_required(VERSION 3.17)
project(CN_CA4)

set(CMAKE_CXX_STANDARD 11)

add_executable(Router Source/Router/main.cpp Source/Common/common.h Source/Common/common.cpp)
add_executable(Receiver Source/Receiver/main.cpp Source/Common/common.h Source/Common/common.cpp)
add_executable(Sender Source/Sender/main.cpp Source/Common/common.h)
