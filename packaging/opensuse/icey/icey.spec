#
# spec file for package icey
#

Name:           icey
Version:        2.4.2
Release:        0
Summary:        C++20 media stack and libwebrtc alternative for real-time video, signalling, TURN, and media servers
License:        LGPL-2.1-or-later
URL:            https://0state.com/icey/
Source0:        %{name}-%{version}.tar.xz
BuildRequires:  c_compiler
BuildRequires:  c++_compiler
BuildRequires:  cmake >= 3.21
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(libavcodec)
BuildRequires:  pkgconfig(libavformat)
BuildRequires:  pkgconfig(libavutil)
BuildRequires:  pkgconfig(libllhttp)
BuildRequires:  pkgconfig(libswresample)
BuildRequires:  pkgconfig(libswscale)
BuildRequires:  pkgconfig(libuv)
BuildRequires:  pkgconfig(minizip)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(zlib)

%description
icey is a modular C++20 media stack and libwebrtc alternative for real-time
video, signalling, TURN, HTTP, and media server workloads.

The openSUSE build keeps FFmpeg enabled for media primitives and deliberately
leaves WebRTC disabled until libdatachannel is packaged for Factory.

%package -n libicey2
Summary:        Runtime libraries for the icey C++20 media stack

%description -n libicey2
This package contains the runtime libraries for the icey C++20 media stack.

%package devel
Summary:        Development files for the icey C++20 media stack
Requires:       libicey2 = %{version}

%description devel
This package contains headers, shared-library symlinks, pkg-config metadata,
and CMake package exports for building against icey.

%prep
%autosetup -p1 -n %{name}-%{version}

%build
%cmake \
  -DBUILD_SHARED_LIBS=ON \
  -DUSE_SYSTEM_DEPS=ON \
  -DBUILD_TESTS=OFF \
  -DBUILD_SAMPLES=OFF \
  -DBUILD_APPLICATIONS=OFF \
  -DBUILD_FUZZERS=OFF \
  -DBUILD_BENCHMARKS=OFF \
  -DBUILD_PERF=OFF \
  -DBUILD_ALPHA=OFF \
  -DCMAKE_DISABLE_FIND_PACKAGE_Doxygen=TRUE \
  -DENABLE_NATIVE_ARCH=OFF \
  -DWITH_FFMPEG=ON \
  -DWITH_LIBDATACHANNEL=OFF \
  -DBUILD_MODULE_webrtc=OFF \
  -DWITH_OPENCV=OFF
%cmake_build

%install
%cmake_install

%ldconfig_scriptlets -n libicey2

%files -n libicey2
%license LICENSE.md
%{_libdir}/libicy_*.so.2.4*

%files devel
%license LICENSE.md
%{_includedir}/icy/
%{_libdir}/libicy_*.so
%{_libdir}/pkgconfig/icey.pc
%{_libdir}/cmake/icey/
