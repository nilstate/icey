from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get, rmdir
from conan.tools.scm import Git
import os


class IceyConan(ConanFile):
    name = "icey"
    version = "2.3.0"
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

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def requirements(self):
        self.requires("openssl/[>=3.0 <4]")
        self.requires("libuv/[>=1.48]")
        self.requires("zlib/[>=1.3]")
        if self.options.with_ffmpeg:
            self.requires("ffmpeg/[>=5.0]")
        if self.options.with_opencv:
            self.requires("opencv/[>=4.5]")

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

        modules = ["base", "net", "crypto", "http", "json", "archo", "sched", "stun", "turn", "symple"]
        if self.options.with_ffmpeg:
            modules.append("av")

        for mod in modules:
            self.cpp_info.components[mod].set_property("cmake_target_name", f"icy_{mod}")
            self.cpp_info.components[mod].libs = [f"icy_{mod}"]
            self.cpp_info.components[mod].includedirs = ["include"]

        # Base depends on libuv
        self.cpp_info.components["base"].requires = ["libuv::libuv"]

        # Net depends on base + openssl
        self.cpp_info.components["net"].requires = ["base", "openssl::openssl"]

        # Crypto depends on net
        self.cpp_info.components["crypto"].requires = ["net"]

        # HTTP depends on net + crypto + zlib
        self.cpp_info.components["http"].requires = ["net", "crypto", "zlib::zlib"]

        # AV depends on base + net + ffmpeg
        if self.options.with_ffmpeg:
            self.cpp_info.components["av"].requires = ["base", "net", "ffmpeg::ffmpeg"]

        # STUN/TURN depend on net + crypto
        self.cpp_info.components["stun"].requires = ["net", "crypto"]
        self.cpp_info.components["turn"].requires = ["net", "crypto", "stun"]

        # Symple depends on net + json + http
        self.cpp_info.components["symple"].requires = ["net", "json", "http"]

        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.components["base"].system_libs = ["pthread", "dl"]
        if self.settings.os == "Windows":
            self.cpp_info.components["base"].system_libs = ["ws2_32", "iphlpapi", "psapi", "userenv"]
