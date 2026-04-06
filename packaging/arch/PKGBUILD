pkgname=icey
pkgver=2.4.3
pkgrel=1
pkgdesc='C++20 media stack and libwebrtc alternative for real-time video, signalling, TURN, and media servers'
arch=('x86_64')
url='https://0state.com/icey/'
license=('LGPL-2.1-or-later')
depends=(
  'ffmpeg'
  'libdatachannel'
  'libuv'
  'llhttp'
  'minizip'
  'openssl'
  'zlib'
)
makedepends=(
  'cmake'
  'pkgconf'
)
source=("${pkgname}-${pkgver}.tar.gz::https://github.com/nilstate/icey/archive/refs/tags/${pkgver}.tar.gz")
sha256sums=('7b78314efd95bed184e203301de10a00c76459dd2f25679e122f733fe927d19a')

build() {
  local cmake_args=(
    -S "${srcdir}/${pkgname}-${pkgver}"
    -B build
    -DCMAKE_BUILD_TYPE=None
    -DCMAKE_INSTALL_PREFIX=/usr
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
  )

  cmake "${cmake_args[@]}"
  cmake --build build
}

package() {
  DESTDIR="${pkgdir}" cmake --install build
  install -Dm644 "${srcdir}/${pkgname}-${pkgver}/LICENSE.md" \
    "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE.md"
}
