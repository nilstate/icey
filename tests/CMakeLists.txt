set(_icey_consumer_tests_dir "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(_icey_consumer_project_dir "${CMAKE_CURRENT_SOURCE_DIR}/consumer")

if(TARGET base AND TARGET net AND TARGET http)
  add_test(
    NAME cmake-buildtree-consumer
    COMMAND ${CMAKE_COMMAND}
      -DICEY_SOURCE_DIR=${CMAKE_SOURCE_DIR}
      -DICEY_BINARY_DIR=${CMAKE_BINARY_DIR}
      -DICEY_CONSUMER_PROJECT_DIR=${_icey_consumer_project_dir}
      -DICEY_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DICEY_C_COMPILER=${CMAKE_C_COMPILER}
      -DICEY_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      "-DICEY_C_FLAGS=${CMAKE_C_FLAGS}"
      "-DICEY_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
      "-DICEY_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
      -P ${_icey_consumer_tests_dir}/buildtree_consumer_test.cmake)

  add_test(
    NAME cmake-installtree-consumer
    COMMAND ${CMAKE_COMMAND}
      -DICEY_SOURCE_DIR=${CMAKE_SOURCE_DIR}
      -DICEY_BINARY_DIR=${CMAKE_BINARY_DIR}
      -DICEY_CONSUMER_PROJECT_DIR=${_icey_consumer_project_dir}
      -DICEY_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DICEY_C_COMPILER=${CMAKE_C_COMPILER}
      -DICEY_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      "-DICEY_C_FLAGS=${CMAKE_C_FLAGS}"
      "-DICEY_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
      "-DICEY_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
      -P ${_icey_consumer_tests_dir}/installtree_consumer_test.cmake)

  set(icey_BUILD_TESTS
    ${icey_BUILD_TESTS}
    cmake-buildtree-consumer
    cmake-installtree-consumer
    CACHE INTERNAL "")
endif()
