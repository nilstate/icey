# WebRTC OpencCV Analyzer

This sample app showcases how to pass live WebRTC video stream to OpenCV to perform object detection.
Demo has next actors:
1. Client - web browser, which is WebRTC source and video player as well (local loopback)
1. Libsourcey server app that connects as WebRTC peer to client's stream.

Libsourcey server app acts as WebRTC listener, OpenCV processor and WebSocket server that sends objects detection data back to a client.
The client app draws received object detections (through separate WebSocket channel) using js to render detections over video loopback HTML element.
This is also interesting demo how to avoid delays in video stream and deliver analytics by separate channel, without extra encoding on server side.

## Installation

1. Compile LibSourcey with WebRTC and OpenCV support (`WITH_WEBRTC=ON WITH_OPENCV=ON`) and samples (`BUILD_SAMPLES_webrtc=ON`) enabled. See the main README for help: https://github.com/sourcey/libsourcey#installing-on-linux
1. Run the `opencvanalyzer` sample application from the `build` directory.
You can rewrite default network settings by arguments, run `opencvanalyzer -h` for help dialog.
1. Install and run the Node.js client application:

~~~ bash
cd <libsourcey>/src/webrtc/samples/opencvanalyzer/client
npm install
node app
~~~

4. Finally, point your browser to: `http://localhost:4499`

IMPORTANT NOTICE!
Firefox: make sure you are using latest master branch of libsourcey, there is an important bugfix for Firefox
Chrome: enable mixed content: `chrome --allow-running-insecure-content`, modify /client/app.js to support HTTPS web server
