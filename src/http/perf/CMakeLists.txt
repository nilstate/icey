add_executable(httpperf httpperf.cpp)
target_link_libraries(httpperf PRIVATE base net http)
target_include_directories(httpperf PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(httpperf PRIVATE ICY_DATA_DIR="${Icey_SOURCE_DIR}/data")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(httpperf PROPERTIES FOLDER "performance")
endif()

add_custom_target(httpperf_compare
  COMMAND "${CMAKE_COMMAND}" -E env bash "${CMAKE_CURRENT_SOURCE_DIR}/benchmark.sh" "$<TARGET_FILE:httpperf>"
  DEPENDS httpperf
  USES_TERMINAL
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(httpperf_compare PROPERTIES FOLDER "performance")
endif()

set(Icey_BUILD_PERF ${Icey_BUILD_PERF} httpperf httpperf_compare CACHE INTERNAL "")
