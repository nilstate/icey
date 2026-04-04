cmake_minimum_required(VERSION 3.21)

project(icey_conan_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(icey REQUIRED COMPONENTS base net http)

add_executable(icey_conan_test src/example.cpp)
target_link_libraries(icey_conan_test PRIVATE icey::base icey::net icey::http)

if(TARGET icey::graft)
  target_link_libraries(icey_conan_test PRIVATE icey::graft)
  target_compile_definitions(icey_conan_test PRIVATE ICEY_HAS_GRAFT=1)
endif()
