//
/// Setup the Symple server
var fs = require('fs');
var Symple = require('symple');
var sy = new Symple();
sy.loadConfig(__dirname + '/symple.json'); // see symple.json for options
sy.init();
console.log('Symple server listening on port ' + sy.config.port);


//
/// Setup the demo client web server

var express = require('express'),
  path = require('path'),
  app = express(),
  serverPort = parseInt(sy.config.port)
clientPort = serverPort - 1;

app.set('port', clientPort);
app.set('view engine', 'ejs');
app.set('views', __dirname + '/');
app.use(express.static(__dirname + '/assets'));


app.get('/', function (req, res) {
  res.render('index', {
    port: serverPort,
    peer: {
      user: 'demo',
      name: 'Demo User',
      group: 'public'
    }
  });
});

app.listen(app.get('port'), function () {
  console.log('Web server listening on port ' + app.get('port'));
});
