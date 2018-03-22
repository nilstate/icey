FROM ubuntu:14.04
MAINTAINER Kam Low <hello@sourcey.com>

# Install the PPA for GCC 6 which is required for C++14
RUN apt-get update && \
  apt-get install -y software-properties-common && \
	add-apt-repository -y ppa:ubuntu-toolchain-r/test

# Install dependencies
RUN apt-get update && apt-get install -y \
  build-essential \
  pkg-config \
  git \
  curl \
  cmake \
  libx11-dev \
  libglu1-mesa-dev \
  gcc-6 \
  g++-6

# Use GCC 6
RUN sudo update-alternatives \
  --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 \
  --slave /usr/bin/g++ g++ /usr/bin/g++-6

# Download and extract precompiled WebRTC static libraries
# COPY vendor/webrtc-22215-ab42706-linux-x64 /vendor/webrtc-22215-ab42706-linux-x64
RUN mkdir -p /vendor/webrtc-22215-ab42706-linux-x64; \
  curl -sSL https://github.com/sourcey/webrtc-precompiled-builds/raw/master/webrtc-22215-ab42706-linux-x64.tar.gz | sudo tar -xzC /vendor/webrtc-22215-ab42706-linux-x64

# Install LibSourcey
RUN git clone https://github.com/sourcey/libsourcey.git && \
  cd /libsourcey && mkdir build && cd build && \
  cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_SHARED_LIBS=OFF -DBUILD_WITH_STATIC_CRT=ON \
           -DBUILD_MODULES=ON -DBUILD_APPLICATIONS=OFF -DBUILD_SAMPLES=OFF -DBUILD_TESTS=OFF \
           -DWITH_FFMPEG=OFF -DWITH_WEBRTC=ON -DENABLE_LOGGING=OFF \
           -DWEBRTC_ROOT_DIR=/vendor/webrtc-22215-ab42706-linux-x64 \
           -DCMAKE_INSTALL_PREFIX=/libsourcey/install && \
  make VERBOSE=1 && \
  make install
  # cachebust

# Set the working directory to the LibSourcey install directory
WORKDIR /libsourcey/install
