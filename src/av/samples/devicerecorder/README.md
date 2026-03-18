# Device Recorder

Records audio and video from system capture devices into an MP4 file using H.264 video and AAC audio codecs.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Requirements

- Camera and/or microphone
- FFmpeg with libx264 and AAC encoder support

## Usage

```
./devicerecorder
```

Starts recording from the default camera (640x480) and microphone to `deviceoutput.mp4` in the working directory. Press Ctrl-C to stop recording and finalize the file.
