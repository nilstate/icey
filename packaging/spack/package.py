from spack.package import *


class Icey(CMakePackage):
    """C++20 media stack and libwebrtc alternative for real-time video,
    signalling, TURN, and media servers."""

    homepage = "https://0state.com/icey/"
    url = "https://github.com/nilstate/icey/archive/refs/tags/2.4.2.tar.gz"

    license("LGPL-2.1-or-later")

    version("2.4.2", sha256="6912ad57336f061e197cc1b969728464a9a658a1d426c67d6af03ef95bff2345")

    variant("ffmpeg", default=True, description="Enable icey::av with FFmpeg")
    variant("webrtc", default=False, description="Enable icey::webrtc with libdatachannel")

    depends_on("cmake@3.21:", type="build")
    depends_on("pkgconfig", type="build")
    depends_on("openssl@3:")
    depends_on("libuv")
    depends_on("llhttp")
    depends_on("minizip")
    depends_on("zlib")
    depends_on("ffmpeg@5:", when="+ffmpeg")
    depends_on("libdatachannel", when="+webrtc")

    def cmake_args(self):
        spec = self.spec
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
            self.define_from_variant("WITH_LIBDATACHANNEL", "webrtc"),
            self.define_from_variant("BUILD_MODULE_webrtc", "webrtc"),
            self.define("WITH_OPENCV", False),
        ]
