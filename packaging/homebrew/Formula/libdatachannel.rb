class Libdatachannel < Formula
  desc "Standalone WebRTC network library featuring data channels and media transport"
  homepage "https://github.com/paullouisageneau/libdatachannel"
  url "https://github.com/paullouisageneau/libdatachannel/archive/refs/tags/v0.24.1.tar.gz"
  sha256 "e6fc363497a41b5dce38602937c12d30e5e536943cf09c5ee5671c8f206eee08"
  license "MPL-2.0"

  depends_on "cmake" => :build
  depends_on "pkgconf" => :build
  depends_on "libnice"
  depends_on "libusrsctp"
  depends_on "nlohmann-json"
  depends_on "openssl@3"
  depends_on "plog"
  depends_on "srtp"

  def install
    args = std_cmake_args + %W[
      -DBUILD_SHARED_LIBS=ON
      -DNO_EXAMPLES=ON
      -DNO_TESTS=ON
      -DNO_WEBSOCKET=ON
      -DPREFER_SYSTEM_LIB=ON
      -DUSE_GNUTLS=OFF
      -DUSE_MBEDTLS=OFF
      -DUSE_NICE=ON
      -DUSE_SYSTEM_JSON=ON
      -DUSE_SYSTEM_PLOG=ON
      -DUSE_SYSTEM_SRTP=ON
      -DUSE_SYSTEM_USRSCTP=ON
      -DOPENSSL_ROOT_DIR=#{Formula["openssl@3"].opt_prefix}
    ]

    system "cmake", "-S", ".", "-B", "build", *args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    assert_predicate include/"rtc/rtc.hpp", :exist?
    assert_predicate lib/"libdatachannel#{shared_library_suffix}", :exist?
  end
end
