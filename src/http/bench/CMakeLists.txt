add_executable(httpbench httpbench.cpp)
target_link_libraries(httpbench PRIVATE base net http)
target_include_directories(httpbench PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(httpbench PRIVATE ICY_DATA_DIR="${Icey_SOURCE_DIR}/data")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(httpbench PROPERTIES FOLDER "benchmarks")
endif()

add_custom_target(httpbench_compare
  COMMAND "${CMAKE_COMMAND}" -E env bash "${CMAKE_CURRENT_SOURCE_DIR}/benchmark.sh" "$<TARGET_FILE:httpbench>"
  DEPENDS httpbench
  USES_TERMINAL
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(httpbench_compare PROPERTIES FOLDER "benchmarks")
endif()

set(Icey_BUILD_BENCHMARKS ${Icey_BUILD_BENCHMARKS} httpbench CACHE INTERNAL "")

icy_add_benchmark(httpparsebench DEPENDS base net http)
icy_add_benchmark(wsbench DEPENDS base net http)
