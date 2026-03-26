from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, rmdir
import os


class IceyConan(ConanFile):
    name = "icey"
    version = "2.3.0"
    package_type = "library"
    license = "LGPL-2.1-or-later"
    url = "https://github.com/sourcey/icey"
    homepage = "https://0state.com/code/icey"
    description = "C++ Networking Evolved - modular C++20 toolkit for networking, media, and real-time communication"
    topics = ("networking", "webrtc", "ffmpeg", "media", "http", "websocket", "stun", "turn", "c++20")

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_ffmpeg": [True, False],
        "with_opencv": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_ffmpeg": False,
        "with_opencv": False,
    }
    no_copy_source = True

    def export_sources(self):
        repo_root = os.path.abspath(os.path.join(self.recipe_folder, "..", ".."))
        src_root = os.path.join(repo_root, "src")
        cmake_root = os.path.join(repo_root, "cmake")
        excludes = (
            "*/node_modules/*",
            "*/dist/*",
            "*/tests/*",
            "*/samples/*",
            "*/apps/*",
            "*/support/*",
        )

        for filename in ("CMakeLists.txt", "Icey.cmake", "VERSION", "LICENSE.md", "README.md", "Doxyfile"):
            copy(self, filename, src=repo_root, dst=self.export_sources_folder)

        copy(self, "*.cmake", src=cmake_root, dst=os.path.join(self.export_sources_folder, "cmake"))
        copy(self, "*.in", src=cmake_root, dst=os.path.join(self.export_sources_folder, "cmake"))
        copy(self, "CMakeLists.txt", src=src_root, dst=os.path.join(self.export_sources_folder, "src"), excludes=excludes)
        copy(self, "*.cpp", src=src_root, dst=os.path.join(self.export_sources_folder, "src"), excludes=excludes)
        copy(self, "*.c", src=src_root, dst=os.path.join(self.export_sources_folder, "src"), excludes=excludes)
        copy(self, "*.h", src=src_root, dst=os.path.join(self.export_sources_folder, "src"), excludes=excludes)
        copy(self, "*.hpp", src=src_root, dst=os.path.join(self.export_sources_folder, "src"), excludes=excludes)
        copy(self, "*.mm", src=src_root, dst=os.path.join(self.export_sources_folder, "src"), excludes=excludes)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def requirements(self):
        self.requires("openssl/[>=3.0 <4]")
        self.requires("libuv/[>=1.48 <2]")
        self.requires("llhttp/[>=9.2 <10]")
        self.requires("minizip/[>=1.3 <2]")
        self.requires("zlib/[>=1.3 <2]")
        if self.options.with_ffmpeg:
            self.requires("ffmpeg/[>=5.0 <8]")
        if self.options.with_opencv:
            self.requires("opencv/[>=4.5 <5]")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["USE_SYSTEM_DEPS"] = True
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["BUILD_TESTS"] = False
        tc.variables["BUILD_SAMPLES"] = False
        tc.variables["BUILD_APPLICATIONS"] = False
        tc.variables["BUILD_FUZZERS"] = False
        tc.variables["BUILD_BENCHMARKS"] = False
        tc.variables["BUILD_ALPHA"] = False
        tc.variables["WITH_LIBDATACHANNEL"] = False
        tc.variables["BUILD_MODULE_webrtc"] = False
        tc.variables["CMAKE_DISABLE_FIND_PACKAGE_Doxygen"] = True
        tc.variables["WITH_FFMPEG"] = self.options.with_ffmpeg
        tc.variables["WITH_OPENCV"] = self.options.with_opencv
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE.md", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "Icey")

        modules = [
            "archo",
            "av",
            "base",
            "crypto",
            "http",
            "json",
            "net",
            "pacm",
            "pluga",
            "sched",
            "stun",
            "symple",
            "turn",
        ]

        for mod in modules:
            self.cpp_info.components[mod].set_property("cmake_target_name", f"Icey::{mod}")
            self.cpp_info.components[mod].libs = [f"icy_{mod}"]
            self.cpp_info.components[mod].includedirs = ["include"]

        self.cpp_info.components["base"].requires = ["libuv::libuv"]
        self.cpp_info.components["archo"].requires = ["base", "minizip::minizip"]
        self.cpp_info.components["av"].requires = ["base"]
        if self.options.with_ffmpeg:
            self.cpp_info.components["av"].requires.append("ffmpeg::ffmpeg")
        self.cpp_info.components["crypto"].requires = ["base", "openssl::openssl"]
        self.cpp_info.components["http"].requires = ["base", "net", "crypto", "llhttp::llhttp", "openssl::openssl"]
        self.cpp_info.components["json"].requires = ["base"]
        self.cpp_info.components["net"].requires = ["base", "crypto", "openssl::openssl"]
        self.cpp_info.components["pacm"].requires = ["base", "crypto", "net", "http", "json", "archo", "openssl::openssl"]
        self.cpp_info.components["pluga"].requires = ["base"]
        self.cpp_info.components["sched"].requires = ["base", "json"]
        self.cpp_info.components["stun"].requires = ["base", "net", "crypto", "openssl::openssl"]
        self.cpp_info.components["turn"].requires = ["base", "net", "crypto", "stun"]
        self.cpp_info.components["symple"].requires = ["base", "crypto", "net", "http", "json", "openssl::openssl"]

        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.components["base"].system_libs = ["pthread", "dl", "m", "rt"]
        if self.settings.os == "Windows":
            self.cpp_info.components["base"].system_libs = ["ws2_32", "iphlpapi", "psapi", "userenv"]
