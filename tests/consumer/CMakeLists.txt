cmake_minimum_required(VERSION 3.21)

project(IceyConsumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Icey REQUIRED COMPONENTS base net http)

add_executable(icey_consumer main.cpp)
target_link_libraries(icey_consumer PRIVATE Icey::base Icey::net Icey::http)
