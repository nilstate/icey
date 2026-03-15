FROM ubuntu:24.04 AS builder
LABEL maintainer="Kam Low <hello@sourcey.com>"

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
  build-essential \
  pkg-config \
  git \
  cmake \
  libssl-dev \
  && rm -rf /var/lib/apt/lists/*

COPY . /libsourcey

RUN cd /libsourcey && mkdir -p build && cd build && \
  cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
           -DBUILD_MODULES=ON -DBUILD_APPLICATIONS=OFF \
           -DBUILD_SAMPLES=OFF -DBUILD_TESTS=OFF \
           -DCMAKE_INSTALL_PREFIX=/libsourcey/install && \
  cmake --build . --parallel "$(nproc)" && \
  cmake --install .

FROM ubuntu:24.04
LABEL maintainer="Kam Low <hello@sourcey.com>"

RUN apt-get update && apt-get install -y --no-install-recommends \
  libssl3t64 \
  && rm -rf /var/lib/apt/lists/*

COPY --from=builder /libsourcey/install /usr/local

WORKDIR /usr/local
