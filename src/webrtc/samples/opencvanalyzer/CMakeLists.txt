include_dependency(WebRTC REQUIRED)
include_dependency(OpenCV REQUIRED)

define_sourcey_module_sample(opencvanalyzer base crypto av net http util json socketio symple webrtc)

configure_file(network/mobilenetssd.caffemodel network/mobilenetssd.caffemodel COPYONLY)

configure_file(network/mobilenetssd.prototxt.txt network/mobilenetssd.prototxt.txt COPYONLY)
