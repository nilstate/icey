var canvas, ctx;
// hardcoded value for video of 640x480, TODO: make it generic
var detectionSize = 480;
var webSocketHost = 'ws://0.0.0.0:1234/';
var clearingTimeout;
function init() {
  // init canvas
  canvas = document.getElementById("detectionCanvas");
  ctx = canvas.getContext("2d");
  ctx.lineWidth="3";
  ctx.strokeStyle="red";
  ctx.font="18px Georgia";
  ctx.fillStyle = "red";

  // init websocket
  initWebSocketConnection();
}

function initWebSocketConnection() {
  websocket = new WebSocket(webSocketHost);
  websocket.onopen = function() { console.log('WebSocket Data Stream Connected'); };
  websocket.onmessage = function(evt) {
    console.log('Detection Data Received:', evt.data);
    drawDetections(JSON.parse(evt.data));
  };
  websocket.onerror = function(evt) {
    console.log('WebSocket ERROR: ' + evt.data);

    // close websocket if opened
    if(websocket.readyState < 2){
      websocket.close();
    }

    // try to reinitialize websocket connection in one second
    setTimeout(initWebSocketConnection, 1000);
  };
}

function drawDetections(data) {
  // remote handler for clearing data if there are no detections from server
  if (clearingTimeout){
    clearTimeout(clearingTimeout);
  }

  // clear previous drawings
  clearDetections();

  //draw detections one by one
  ctx.beginPath();
  for(var i=0; i<data.detections.length; i++) {
    var detection = data.detections[i];

    ctx.rect(detection.box.leftBottomX + 10,
      detection.box.leftBottomY - 10,
      detection.box.rightTopX - detection.box.leftBottomX - 30,
      detection.box.rightTopY - detection.box.leftBottomY -  10);
    ctx.fillText(detection.class + ' ' + detection.confidence,
      detection.box.leftBottomX + 15, detection.box.leftBottomY + 15);
    ctx.stroke();
  }
  ctx.closePath();

  // set timer for clearing previous detection drawings
  // if there are no new detections from server
  clearingTimeout = setTimeout(clearDetections, 1000);
}

function clearDetections() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
}
window.addEventListener("load", init, false);
