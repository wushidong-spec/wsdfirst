cmake_minimum_required(VERSION 3.24)
project(untitled2)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled2 main.cpp TaskQueue.cpp STimer.cpp DataStream.h DataStream.h baseheader.h FunctionHandler.h server.h)
target_link_libraries(untitled2 PRIVATE ws2_32)