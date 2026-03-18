# Device Capture

Opens a camera using capability-aware parameter negotiation and captures frames for 5 seconds. Demonstrates `negotiateVideoCapture` for selecting the best device-supported resolution.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Requirements

- Camera connected to the system
- FFmpeg libraries

## Usage

```
./devicecapture [camera-name]
```

If no camera name is given, uses the default camera. Prints device capabilities, opens at the best match for 720p @ 30fps, and captures for 5 seconds.
