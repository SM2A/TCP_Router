cmake_minimum_required(VERSION 3.17)
project(CN_CA4)

set(CMAKE_CXX_STANDARD 11)

add_executable(Router Source/Router/main.cpp)
add_executable(Receiver Source/Receiver/main.cpp)
add_executable(Sender Source/Sender/main.cpp)

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#target_link_libraries(Router PRIVATE Threads::Threads)
