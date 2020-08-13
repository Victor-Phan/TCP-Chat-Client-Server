cmake_minimum_required(VERSION 3.15)
project(COMP4981_Assignment3)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(COMP4981_Assignment3 main.cpp client.cpp filehandler.cpp Server.cpp Server.h main.h)