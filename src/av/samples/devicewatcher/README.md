# Device Watcher

Demonstrates hot-plug detection and device change monitoring using the platform-specific DeviceWatcher.

## Build

Built automatically when `-DBUILD_SAMPLES=ON` is passed to CMake.

## Requirements

- No hardware required (monitors system device events)

## Usage

```
./devicewatcher
```

Prints initial device state, then watches for device add/remove events. Plug or unplug a camera or microphone to see change notifications. Press Ctrl-C to exit.
