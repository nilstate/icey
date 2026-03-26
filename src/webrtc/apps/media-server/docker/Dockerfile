FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ARG ICEY_MEDIA_SERVER_BINARY=build-webrtc/webrtc/apps/media-server/media-server

RUN apt-get update && apt-get install -y --no-install-recommends \
  ca-certificates \
  ffmpeg \
  libssl3t64 \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN mkdir -p /app/media /app/recordings

COPY ${ICEY_MEDIA_SERVER_BINARY} /usr/local/bin/media-server
COPY src/webrtc/apps/media-server/web/dist /app/web
COPY data/test.mp4 /app/media/test.mp4
COPY src/webrtc/apps/media-server/docker/entrypoint.sh /app/entrypoint.sh

RUN chmod +x /app/entrypoint.sh

EXPOSE 4500 3478/tcp 3478/udp

ENTRYPOINT ["/app/entrypoint.sh"]
