cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED ICEY_SOURCE_DIR OR NOT DEFINED ICEY_BINARY_DIR OR NOT DEFINED ICEY_CONSUMER_PROJECT_DIR)
  message(FATAL_ERROR "Missing required consumer-test paths")
endif()

set(_consumer_build_dir "${ICEY_BINARY_DIR}/consumer-buildtree")
file(REMOVE_RECURSE "${_consumer_build_dir}")

execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build "${ICEY_BINARY_DIR}"
    --target http
    --config ${ICEY_BUILD_TYPE}
  RESULT_VARIABLE _icey_build_result
)
if(NOT _icey_build_result EQUAL 0)
  message(FATAL_ERROR "Failed to build icey before build-tree consumer test")
endif()

set(_consumer_cmake_args
  -S "${ICEY_CONSUMER_PROJECT_DIR}"
  -B "${_consumer_build_dir}"
  -DCMAKE_BUILD_TYPE=${ICEY_BUILD_TYPE}
  -DCMAKE_PREFIX_PATH=${ICEY_BINARY_DIR}
)
if(DEFINED ICEY_C_COMPILER)
  list(APPEND _consumer_cmake_args -DCMAKE_C_COMPILER=${ICEY_C_COMPILER})
endif()
if(DEFINED ICEY_CXX_COMPILER)
  list(APPEND _consumer_cmake_args -DCMAKE_CXX_COMPILER=${ICEY_CXX_COMPILER})
endif()
if(DEFINED ICEY_C_FLAGS)
  list(APPEND _consumer_cmake_args "-DCMAKE_C_FLAGS=${ICEY_C_FLAGS}")
endif()
if(DEFINED ICEY_CXX_FLAGS)
  list(APPEND _consumer_cmake_args "-DCMAKE_CXX_FLAGS=${ICEY_CXX_FLAGS}")
endif()
if(DEFINED ICEY_EXE_LINKER_FLAGS)
  list(APPEND _consumer_cmake_args "-DCMAKE_EXE_LINKER_FLAGS=${ICEY_EXE_LINKER_FLAGS}")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} ${_consumer_cmake_args}
  RESULT_VARIABLE _configure_result
)
if(NOT _configure_result EQUAL 0)
  message(FATAL_ERROR "Failed to configure build-tree consumer")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build "${_consumer_build_dir}"
    --config ${ICEY_BUILD_TYPE}
  RESULT_VARIABLE _build_result
)
if(NOT _build_result EQUAL 0)
  message(FATAL_ERROR "Failed to build build-tree consumer")
endif()

set(_consumer_exe "${_consumer_build_dir}/icey_consumer")
if(WIN32)
  set(_consumer_exe "${_consumer_build_dir}/Release/icey_consumer.exe")
endif()

execute_process(
  COMMAND "${_consumer_exe}"
  RESULT_VARIABLE _run_result
)
if(NOT _run_result EQUAL 0)
  message(FATAL_ERROR "Build-tree consumer executable failed")
endif()
