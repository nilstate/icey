from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get, rmdir
import glob
import os


class IceyConan(ConanFile):
    name = "icey"
    package_type = "library"
    license = "LGPL-2.1-or-later"
    author = "0state OSS <oss@0state.com>"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://0state.com/icey/"
    description = "C++20 media stack and libwebrtc alternative for real-time video, signalling, TURN, and media servers"
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

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def requirements(self):
        # icey public headers expose these dependencies directly, so static consumers
        # need their include paths propagated transitively.
        self.requires("openssl/[>=3.0 <4]", transitive_headers=True)
        self.requires("libuv/[>=1.48 <2]", transitive_headers=True)
        self.requires("llhttp/[>=9.2 <10]", transitive_headers=True)
        self.requires("minizip/[>=1.3 <2]", transitive_headers=True)
        if self.options.with_ffmpeg:
            self.requires("ffmpeg/[>=5.0 <8]", transitive_headers=True)
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
        tc.variables["ENABLE_NATIVE_ARCH"] = False
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
        self.cpp_info.set_property("cmake_file_name", "icey")

        libdir = os.path.join(self.package_folder, "lib")
        includedir = os.path.join(self.package_folder, "include", "icy")

        def has_library(stem):
            patterns = [
                os.path.join(libdir, f"lib{stem}.*"),
                os.path.join(libdir, f"{stem}.*"),
            ]
            return any(glob.glob(pattern) for pattern in patterns)

        def has_headers(module):
            return os.path.isdir(os.path.join(includedir, module))

        def add_component(module, requires=None, header_only=False):
            if header_only:
                if not has_headers(module):
                    return False
            elif not has_library(f"icy_{module}"):
                return False

            component = self.cpp_info.components[module]
            component.set_property("cmake_target_name", f"icey::{module}")
            component.includedirs = ["include"]
            if header_only:
                component.libdirs = []
            else:
                component.libs = [f"icy_{module}"]
            if requires:
                component.requires = requires
            return True

        add_component("base", ["libuv::libuv"])
        if self.settings.os in ["Linux", "FreeBSD"] and "base" in self.cpp_info.components:
            self.cpp_info.components["base"].system_libs = ["pthread", "dl", "m", "rt"]
        if self.settings.os == "Windows" and "base" in self.cpp_info.components:
            self.cpp_info.components["base"].system_libs = ["ws2_32", "iphlpapi", "psapi", "userenv"]

        av_requires = ["base"]
        if self.options.with_ffmpeg:
            av_requires.append("ffmpeg::ffmpeg")

        add_component("archo", ["base", "minizip::minizip"])
        add_component("av", av_requires)
        add_component("crypto", ["base", "openssl::openssl"])
        add_component("http", ["base", "net", "crypto", "llhttp::llhttp", "openssl::openssl"])
        add_component("json", ["base"])
        add_component("net", ["base", "crypto", "openssl::openssl"])
        add_component("pacm", ["base", "crypto", "net", "http", "json", "archo", "openssl::openssl"])
        add_component("graft", ["base"])
        add_component("pluga", ["base"], header_only=True)
        add_component("sched", ["base", "json"])
        add_component("speech", ["base", "av", "json"])
        add_component("stun", ["base", "net", "crypto", "openssl::openssl"])
        add_component("symple", ["base", "crypto", "net", "http", "json", "openssl::openssl"])
        add_component("turn", ["base", "net", "crypto", "stun"])
        add_component("vision", ["base", "av", "json"])
