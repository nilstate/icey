cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED ICEY_SOURCE_DIR OR NOT DEFINED ICEY_BINARY_DIR OR NOT DEFINED ICEY_CONSUMER_PROJECT_DIR)
  message(FATAL_ERROR "Missing required consumer-test paths")
endif()

set(_package_build_dir "${ICEY_BINARY_DIR}/consumer-package-build")
set(_consumer_install_prefix "${ICEY_BINARY_DIR}/consumer-install")
set(_consumer_build_dir "${ICEY_BINARY_DIR}/consumer-install-build")
file(REMOVE_RECURSE
  "${_package_build_dir}"
  "${_consumer_install_prefix}"
  "${_consumer_build_dir}")

execute_process(
  COMMAND ${CMAKE_COMMAND}
    -S "${ICEY_SOURCE_DIR}"
    -B "${_package_build_dir}"
    -DCMAKE_BUILD_TYPE=Release
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
  RESULT_VARIABLE _configure_result
)
if(NOT _configure_result EQUAL 0)
  message(FATAL_ERROR "Failed to configure slim icey package build")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build "${_package_build_dir}"
    --config Release
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

execute_process(
  COMMAND ${CMAKE_COMMAND}
    -S "${ICEY_CONSUMER_PROJECT_DIR}"
    -B "${_consumer_build_dir}"
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_PREFIX_PATH=${_consumer_install_prefix}
  RESULT_VARIABLE _consumer_configure_result
)
if(NOT _consumer_configure_result EQUAL 0)
  message(FATAL_ERROR "Failed to configure install-tree consumer")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND}
    --build "${_consumer_build_dir}"
    --config Release
  RESULT_VARIABLE _consumer_build_result
)
if(NOT _consumer_build_result EQUAL 0)
  message(FATAL_ERROR "Failed to build install-tree consumer")
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
  message(FATAL_ERROR "Install-tree consumer executable failed")
endif()
