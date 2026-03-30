from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, rmdir
import os


class iceyConan(ConanFile):
    name = "icey"
    version = "2.4.0"
    package_type = "library"
    license = "LGPL-2.1-or-later"
    author = "0state OSS <oss@0state.com>"
    url = "https://github.com/nilstate/icey"
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

    def export_sources(self):
        repo_root = os.path.abspath(os.path.join(self.recipe_folder, "..", ".."))
        src_root = os.path.join(repo_root, "src")
        cmake_root = os.path.join(repo_root, "cmake")
        skip_dirs = {"node_modules", "dist", "build", "tests", "samples", "apps", "support"}
        excludes = (
            "*/node_modules/*",
            "*/dist/*",
            "*/build/*",
            "*/tests/*",
            "*/samples/*",
            "*/apps/*",
            "*/support/*",
        )

        for filename in ("icey.cmake", "VERSION", "LICENSE.md", "README.md", "Doxyfile"):
            copy(self, filename, src=repo_root, dst=self.export_sources_folder)

        for root, dirs, files in os.walk(repo_root):
            dirs[:] = [dirname for dirname in dirs if dirname not in skip_dirs and dirname != ".git"]
            if "CMakeLists.txt" not in files:
                continue

            rel_root = os.path.relpath(root, repo_root)
            dst = self.export_sources_folder if rel_root == "." else os.path.join(self.export_sources_folder, rel_root)
            copy(self, "CMakeLists.txt", src=root, dst=dst, keep_path=False)

        copy(self, "*.cmake", src=cmake_root, dst=os.path.join(self.export_sources_folder, "cmake"))
        copy(self, "*.in", src=cmake_root, dst=os.path.join(self.export_sources_folder, "cmake"))
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

        modules = [
            "archo",
            "av",
            "base",
            "crypto",
            "http",
            "json",
            "net",
            "pacm",
            "graft",
            "sched",
            "stun",
            "symple",
            "turn",
        ]

        for mod in modules:
            self.cpp_info.components[mod].set_property("cmake_target_name", f"icey::{mod}")
            self.cpp_info.components[mod].includedirs = ["include"]
            self.cpp_info.components[mod].libs = [f"icy_{mod}"]

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
        self.cpp_info.components["graft"].requires = ["base"]
        self.cpp_info.components["sched"].requires = ["base", "json"]
        self.cpp_info.components["stun"].requires = ["base", "net", "crypto", "openssl::openssl"]
        self.cpp_info.components["turn"].requires = ["base", "net", "crypto", "stun"]
        self.cpp_info.components["symple"].requires = ["base", "crypto", "net", "http", "json", "openssl::openssl"]

        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.components["base"].system_libs = ["pthread", "dl", "m", "rt"]
        if self.settings.os == "Windows":
            self.cpp_info.components["base"].system_libs = ["ws2_32", "iphlpapi", "psapi", "userenv"]
