cmake_minimum_required(VERSION 3.21)

project(icey_consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(icey REQUIRED COMPONENTS base net http)

add_executable(icey_consumer main.cpp)
target_link_libraries(icey_consumer PRIVATE icey::base icey::net icey::http)
