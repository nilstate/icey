#
# spec file for package llhttp
#

%define sover 9_3

Name:           llhttp
Version:        9.3.1
Release:        0
Summary:        Port of http_parser to llparse
License:        MIT
URL:            https://github.com/nodejs/llhttp
Source0:        %{name}-%{version}.tar.xz
BuildRequires:  c_compiler
BuildRequires:  c++_compiler
BuildRequires:  cmake >= 3.25
BuildRequires:  pkgconfig

%description
llhttp is a port of http_parser to llparse. It provides a fast, maintainable
HTTP request and response parser with both pkg-config and CMake package
metadata for downstream consumers.

%package -n libllhttp%{sover}
Summary:        Shared library for llhttp

%description -n libllhttp%{sover}
This package contains the llhttp shared library.

%package devel
Summary:        Development files for llhttp
Requires:       libllhttp%{sover} = %{version}

%description devel
This package contains development files for llhttp, including headers,
pkg-config metadata, and CMake package exports.

%prep
%autosetup -p1 -n %{name}-%{version}

%build
%cmake \
  -DLLHTTP_BUILD_SHARED_LIBS=ON \
  -DLLHTTP_BUILD_STATIC_LIBS=OFF
%cmake_build

%install
%cmake_install

%ldconfig_scriptlets -n libllhttp%{sover}

%files -n libllhttp%{sover}
%license LICENSE LICENSE-MIT
%{_libdir}/libllhttp.so.9.3*

%files devel
%license LICENSE LICENSE-MIT
%doc README.md
%{_includedir}/llhttp.h
%{_libdir}/libllhttp.so
%{_libdir}/pkgconfig/libllhttp.pc
%{_libdir}/cmake/llhttp/
