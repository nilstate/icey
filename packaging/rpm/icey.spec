Name:           icey
Version:        2.4.2
Release:        1%{?dist}
Summary:        C++20 media stack and libwebrtc alternative for real-time video, signalling, TURN, and media servers

License:        LGPL-2.1-or-later
URL:            https://0state.com/icey/
Source0:        https://github.com/nilstate/icey/archive/refs/tags/%{version}.tar.gz#/%{name}-%{version}.tar.gz
Patch0:         fix-libuv-shared-selection.patch

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  pkgconf-pkg-config
BuildRequires:  ffmpeg-free-devel
BuildRequires:  libuv-devel
BuildRequires:  llhttp-devel
BuildRequires:  minizip-ng-compat-devel
BuildRequires:  openssl-devel
BuildRequires:  zlib-devel

%description
icey is a modular C++20 media stack and libwebrtc alternative for real-time
video, signalling, TURN, HTTP, and media server workloads.

The RPM build keeps FFmpeg enabled for media primitives and deliberately leaves
WebRTC disabled until libdatachannel packaging is standardized for Fedora / OBS.

%package devel
Summary:        Development files for the icey C++20 media stack
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Development headers, CMake package exports, and pkg-config metadata for
building applications and services against icey.

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

%files
%license LICENSE.md
%{_libdir}/libicy_*.so.*

%files devel
%{_includedir}/icy/
%{_includedir}/icey.h
%{_includedir}/nlohmann/json.hpp
%{_libdir}/libicy_*.so
%{_libdir}/pkgconfig/icey.pc
%{_libdir}/cmake/icey/

%changelog
* Sat Apr 04 2026 0state OSS <oss@0state.com> - 2.4.2-1
- Refresh RPM packaging for icey 2.4.2
- Backport the shared-libuv system dependency fix for Fedora builds
- Claim installed umbrella and nlohmann development headers

* Mon Mar 30 2026 0state OSS <oss@0state.com> - 2.4.0-1
- Initial RPM packaging for Fedora and OBS
