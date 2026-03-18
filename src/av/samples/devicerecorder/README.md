# Device Recorder

Records audio and video from system capture devices into an MP4 file using H.264 video and AAC audio codecs. Uses `negotiateVideoCapture` and `negotiateAudioCapture` to select the best parameters the device supports.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Requirements

- Camera and/or microphone
- FFmpeg with libx264 and AAC encoder support

## Usage

```
./devicerecorder
```

Negotiates the best parameters from the default camera (targeting 720p @ 30fps) and microphone (44100 Hz stereo), then records to `deviceoutput.mp4` in the working directory. Press Ctrl-C to stop recording and finalize the file.
