add_executable(httpbench ../samples/httpbenchmark/httpbenchmark.cpp)
target_link_libraries(httpbench PRIVATE base net http)
target_include_directories(httpbench PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(httpbench PRIVATE ICY_DATA_DIR="${Icey_SOURCE_DIR}/data")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(httpbench PROPERTIES FOLDER "benchmarks")
endif()

set(Icey_BUILD_BENCHMARKS ${Icey_BUILD_BENCHMARKS} httpbench CACHE INTERNAL "")
