cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED ICEY_SOURCE_DIR OR NOT DEFINED ICEY_BINARY_DIR OR NOT DEFINED ICEY_CONSUMER_PROJECT_DIR)
  message(FATAL_ERROR "Missing required consumer-test paths")
endif()

if(NOT DEFINED ICEY_BUILD_TYPE OR ICEY_BUILD_TYPE STREQUAL "")
  set(ICEY_BUILD_TYPE Release)
endif()

set(_package_build_dir "${ICEY_BINARY_DIR}/consumer-package-build")
set(_consumer_install_prefix "${ICEY_BINARY_DIR}/consumer-install")
set(_consumer_build_dir "${ICEY_BINARY_DIR}/consumer-install-build")
file(REMOVE_RECURSE
  "${_package_build_dir}"
  "${_consumer_install_prefix}"
  "${_consumer_build_dir}")

set(_package_cmake_args
  -S "${ICEY_SOURCE_DIR}"
  -B "${_package_build_dir}"
  -DCMAKE_BUILD_TYPE=${ICEY_BUILD_TYPE}
  -DBUILD_APPLICATIONS=OFF
  -DBUILD_SAMPLES=OFF
  -DBUILD_TESTS=OFF
  -DBUILD_FUZZERS=OFF
  -DBUILD_ALPHA=OFF
  -DBUILD_MODULE_archo=OFF
  -DBUILD_MODULE_av=OFF
  -DBUILD_MODULE_json=OFF
  -DBUILD_MODULE_pacm=OFF
  -DBUILD_MODULE_pluga=OFF
  -DBUILD_MODULE_sched=OFF
  -DBUILD_MODULE_stun=OFF
  -DBUILD_MODULE_symple=OFF
  -DBUILD_MODULE_turn=OFF
  -DBUILD_MODULE_webrtc=OFF
)
if(DEFINED ICEY_C_COMPILER)
  list(APPEND _package_cmake_args -DCMAKE_C_COMPILER=${ICEY_C_COMPILER})
endif()
if(DEFINED ICEY_CXX_COMPILER)
  list(APPEND _package_cmake_args -DCMAKE_CXX_COMPILER=${ICEY_CXX_COMPILER})
endif()
if(DEFINED ICEY_C_FLAGS)
  list(APPEND _package_cmake_args "-DCMAKE_C_FLAGS=${ICEY_C_FLAGS}")
endif()
if(DEFINED ICEY_CXX_FLAGS)
  list(APPEND _package_cmake_args "-DCMAKE_CXX_FLAGS=${ICEY_CXX_FLAGS}")
endif()
if(DEFINED ICEY_EXE_LINKER_FLAGS)
  list(APPEND _package_cmake_args "-DCMAKE_EXE_LINKER_FLAGS=${ICEY_EXE_LINKER_FLAGS}")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} ${_package_cmake_args}
  RESULT_VARIABLE _configure_result
)
if(NOT _configure_result EQUAL 0)
  message(FATAL_ERROR "Failed to configure slim icey package build")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build "${_package_build_dir}"
    --config ${ICEY_BUILD_TYPE}
  RESULT_VARIABLE _build_result
)
if(NOT _build_result EQUAL 0)
  message(FATAL_ERROR "Failed to build slim icey package build")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND}
    --install "${_package_build_dir}"
    --prefix "${_consumer_install_prefix}"
  RESULT_VARIABLE _install_result
)
if(NOT _install_result EQUAL 0)
  message(FATAL_ERROR "Failed to install icey for install-tree consumer test")
endif()

set(_consumer_cmake_args
  -S "${ICEY_CONSUMER_PROJECT_DIR}"
  -B "${_consumer_build_dir}"
  -DCMAKE_BUILD_TYPE=${ICEY_BUILD_TYPE}
  -DCMAKE_PREFIX_PATH=${_consumer_install_prefix}
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
  RESULT_VARIABLE _consumer_configure_result
)
if(NOT _consumer_configure_result EQUAL 0)
  message(FATAL_ERROR "Failed to configure install-tree consumer")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build "${_consumer_build_dir}"
    --config ${ICEY_BUILD_TYPE}
  RESULT_VARIABLE _consumer_build_result
)
if(NOT _consumer_build_result EQUAL 0)
  message(FATAL_ERROR "Failed to build install-tree consumer")
endif()

set(_consumer_exe "${_consumer_build_dir}/icey_consumer")
if(WIN32)
  set(_consumer_exe "${_consumer_build_dir}/${ICEY_BUILD_TYPE}/icey_consumer.exe")
endif()

execute_process(
  COMMAND "${_consumer_exe}"
  RESULT_VARIABLE _run_result
)
if(NOT _run_result EQUAL 0)
  message(FATAL_ERROR "Install-tree consumer executable failed")
endif()
