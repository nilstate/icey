class Icey < Formula
  desc "C++20 media stack and libwebrtc alternative for real-time video, signalling, TURN, and media servers"
  homepage "https://0state.com/icey/"
  url "https://github.com/nilstate/icey/archive/refs/tags/2.4.0.tar.gz"
  version "2.4.0"
  sha256 "0c402b56093f392613b61320087bb888d291e80cdcb038dbb360555b6e1b6cc3"
  license "LGPL-2.1-or-later"

  depends_on "cmake" => :build
  depends_on "pkgconf" => :build
  depends_on "ffmpeg"
  depends_on "libuv"
  depends_on "llhttp"
  depends_on "minizip"
  depends_on "openssl@3"
  depends_on "libdatachannel"

  def install
    args = std_cmake_args + %W[
      -DBUILD_SHARED_LIBS=ON
      -DUSE_SYSTEM_DEPS=ON
      -DBUILD_TESTS=OFF
      -DBUILD_SAMPLES=OFF
      -DBUILD_APPLICATIONS=OFF
      -DBUILD_FUZZERS=OFF
      -DBUILD_BENCHMARKS=OFF
      -DBUILD_PERF=OFF
      -DBUILD_ALPHA=OFF
      -DCMAKE_DISABLE_FIND_PACKAGE_Doxygen=TRUE
      -DENABLE_NATIVE_ARCH=OFF
      -DWITH_FFMPEG=ON
      -DWITH_LIBDATACHANNEL=ON
      -DWITH_OPENCV=OFF
      -DOPENSSL_ROOT_DIR=#{Formula["openssl@3"].opt_prefix}
    ]

    system "cmake", "-S", ".", "-B", "build", *args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    assert_predicate include/"icy/base/base.h", :exist?
    assert_predicate lib/"pkgconfig/icey.pc", :exist?
    assert_predicate lib/"cmake/icey/iceyConfig.cmake", :exist?
  end
end
