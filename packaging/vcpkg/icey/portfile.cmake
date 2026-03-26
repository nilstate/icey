if(DEFINED ENV{ICEY_VCPKG_SOURCE_PATH} AND NOT "$ENV{ICEY_VCPKG_SOURCE_PATH}" STREQUAL "")
    get_filename_component(SOURCE_PATH "$ENV{ICEY_VCPKG_SOURCE_PATH}" ABSOLUTE)
    message(STATUS "Using local icey source tree: ${SOURCE_PATH}")
else()
    # Keep the local overlay workflow pointed at the checked-out tree while
    # release publication catches up. Before upstream submission, update this
    # REF/SHA512 pair to the pushed 2.3.0 git tag archive.
    vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO sourcey/icey
        REF "23ef5b8cc38f56fbe2c89020337f1488ae7c5213"
        SHA512 975b2361d9127483cbd590266bd050c7aedbf1078f2db8756795fbcf4893add621fa4b48a75991715cee1d2e13fb8494e3bdfcbbce2a857eadc7c5e7be3493d5
        HEAD_REF master
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
