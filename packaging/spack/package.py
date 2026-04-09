from spack_repo.builtin.build_systems.cmake import CMakePackage

from spack.package import *


class Icey(CMakePackage):
    """C++20 media stack and libwebrtc alternative for real-time video,
    signalling, TURN, and media servers."""

    homepage = "https://0state.com/icey/"
    url = "https://github.com/nilstate/icey/archive/refs/tags/2.4.4.tar.gz"
    git = "https://github.com/nilstate/icey.git"

    license("LGPL-2.1-or-later")

    version("2.4.4", sha256="f6a09dede14f7b53d763a3284559ad3a42de2bac35586821f84a6a3c9caef71f")

    depends_on("c", type="build")
    depends_on("cxx", type="build")
    variant("ffmpeg", default=True, description="Enable icey::av with FFmpeg")

    depends_on("cmake@3.21:", type="build")
    depends_on("pkgconfig", type="build")
    depends_on("openssl@3:")
    depends_on("libuv")
    depends_on("llhttp")
    depends_on("minizip")
    depends_on("zlib")
    depends_on("ffmpeg@5:", when="+ffmpeg")

    def cmake_args(self):
        return [
            self.define("BUILD_SHARED_LIBS", True),
            self.define("USE_SYSTEM_DEPS", True),
            self.define("BUILD_TESTS", False),
            self.define("BUILD_SAMPLES", False),
            self.define("BUILD_APPLICATIONS", False),
            self.define("BUILD_FUZZERS", False),
            self.define("BUILD_BENCHMARKS", False),
            self.define("BUILD_PERF", False),
            self.define("BUILD_ALPHA", False),
            self.define("CMAKE_DISABLE_FIND_PACKAGE_Doxygen", True),
            self.define("ENABLE_NATIVE_ARCH", False),
            self.define_from_variant("WITH_FFMPEG", "ffmpeg"),
            self.define("WITH_LIBDATACHANNEL", False),
            self.define("BUILD_MODULE_webrtc", False),
            self.define("WITH_OPENCV", False),
        ]
