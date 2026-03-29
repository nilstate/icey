{ lib
, stdenv
, cmake
, pkg-config
, openssl
, libuv
, llhttp
, minizip
, zlib
, ffmpeg ? null
, libdatachannel ? null
, src
, version
, withFfmpeg ? true
, withWebrtc ? false
}:

stdenv.mkDerivation {
  pname = "icey";
  inherit version src;

  outputs = [ "out" "dev" ];

  nativeBuildInputs = [
    cmake
    pkg-config
  ];

  buildInputs =
    [
      openssl
      libuv
      llhttp
      minizip
      zlib
    ]
    ++ lib.optional withFfmpeg ffmpeg
    ++ lib.optional withWebrtc libdatachannel;

  cmakeFlags = [
    "-DBUILD_SHARED_LIBS=ON"
    "-DUSE_SYSTEM_DEPS=ON"
    "-DBUILD_TESTS=OFF"
    "-DBUILD_SAMPLES=OFF"
    "-DBUILD_APPLICATIONS=OFF"
    "-DBUILD_FUZZERS=OFF"
    "-DBUILD_BENCHMARKS=OFF"
    "-DBUILD_PERF=OFF"
    "-DBUILD_ALPHA=OFF"
    "-DCMAKE_DISABLE_FIND_PACKAGE_Doxygen=TRUE"
    "-DENABLE_NATIVE_ARCH=OFF"
    "-DWITH_FFMPEG=${if withFfmpeg then "ON" else "OFF"}"
    "-DWITH_LIBDATACHANNEL=${if withWebrtc then "ON" else "OFF"}"
    "-DBUILD_MODULE_webrtc=${if withWebrtc then "ON" else "OFF"}"
    "-DWITH_OPENCV=OFF"
  ];

  doCheck = false;

  postInstall = ''
    if [ -d "$out/include" ]; then
      moveToOutput include "$dev"
    fi
    if [ -d "$out/lib/cmake" ]; then
      moveToOutput lib/cmake "$dev"
    fi
    if [ -d "$out/lib/pkgconfig" ]; then
      moveToOutput lib/pkgconfig "$dev"
    fi
    shopt -s nullglob
    for archive in "$out"/lib/*.a; do
      moveToOutput "''${archive#$out/}" "$dev"
    done
  '';

  meta = with lib; {
    description = "C++20 media stack and libwebrtc alternative for real-time video, signalling, TURN, and media servers";
    homepage = "https://0state.com/icey/";
    license = licenses.lgpl21Plus;
    platforms = platforms.linux ++ platforms.darwin;
  };
}
