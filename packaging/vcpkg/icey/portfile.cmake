if(DEFINED ENV{ICEY_VCPKG_SOURCE_PATH} AND NOT "$ENV{ICEY_VCPKG_SOURCE_PATH}" STREQUAL "")
    get_filename_component(SOURCE_PATH "$ENV{ICEY_VCPKG_SOURCE_PATH}" ABSOLUTE)
    message(STATUS "Using local icey source tree: ${SOURCE_PATH}")
else()
    vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO nilstate/icey
        REF "2.4.2"
        SHA512 1b56b3c9f082d4d35f9150c75c1c23972c696f853f3f0627787f33882e07c44de843b3896dc4a54f04cefd20dd7b4a6f64a52c5cdabc11d98b5be57c17193dfb
        HEAD_REF main
    )
endif()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        ffmpeg   WITH_FFMPEG
        opencv   WITH_OPENCV
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DUSE_SYSTEM_DEPS=ON
        -DBUILD_TESTS=OFF
        -DBUILD_SAMPLES=OFF
        -DBUILD_APPLICATIONS=OFF
        -DBUILD_FUZZERS=OFF
        -DBUILD_BENCHMARKS=OFF
        -DBUILD_ALPHA=OFF
        -DWITH_LIBDATACHANNEL=OFF
        -DCMAKE_DISABLE_FIND_PACKAGE_Doxygen=TRUE
        ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/icey)
vcpkg_fixup_pkgconfig()
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")
