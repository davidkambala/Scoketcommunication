cmake_minimum_required(VERSION 3.30)
project(SocketCommunicatiob)


find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)

add_executable(SocketCommunicatiob main.cpp)
target_link_libraries(SocketCommunicatiob ${OpenCV_LIBS})
