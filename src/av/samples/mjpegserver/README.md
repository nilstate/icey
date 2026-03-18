# Video Socket

HTTP server that streams MJPEG video from a capture device using multipart HTTP responses. Each connected client gets an independently encoded video stream.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Requirements

- Camera (or a test video file if `USE_AVDEVICE_CAPTURE` is enabled)
- FFmpeg with MJPEG encoder support

## Usage

```
./videosocket
```

Starts an HTTP server on port 328. Connect with a browser or HTTP client to receive a multipart MJPEG stream (400x300 at 25fps). Press Ctrl-C to stop.

Set `USE_AVDEVICE_CAPTURE` to `1` in the source to stream from a file instead of a camera.
