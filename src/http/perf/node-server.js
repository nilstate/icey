// Node.js HTTP benchmark servers for comparison with icey.
// Usage: node node-server.js [minimal|echo]

const http = require('http');
const cluster = require('cluster');
const os = require('os');

const PORT = 1337;
const mode = process.argv[2] || 'minimal';

if (mode === 'minimal') {
  // Minimal response benchmark (matches icey benchmark server)
  const server = http.createServer((req, res) => {
    res.writeHead(200, {
      'Content-Length': '0',
      'Connection': 'close'
    });
    res.end();
  });

  server.listen(PORT, () => {
    console.log(`Node.js HTTP benchmark (minimal, pid ${process.pid}) listening on port ${PORT}`);
  });

} else if (mode === 'echo') {
  // Echo server (matches icey echo server)
  const server = http.createServer((req, res) => {
    req.pipe(res);
  });

  server.listen(PORT, () => {
    console.log(`Node.js HTTP echo (pid ${process.pid}) listening on port ${PORT}`);
  });

} else if (mode === 'keepalive') {
  // Keep-alive benchmark (HTTP/1.1 default - no Connection: close)
  const server = http.createServer((req, res) => {
    res.writeHead(200, { 'Content-Length': '0' });
    res.end();
  });

  server.listen(PORT, () => {
    console.log(`Node.js HTTP benchmark (keep-alive, pid ${process.pid}) listening on port ${PORT}`);
  });

} else if (mode === 'cluster') {
  // Clustered minimal response (one worker per CPU)
  if (cluster.isPrimary) {
    const ncpus = os.cpus().length;
    console.log(`Node.js HTTP benchmark (cluster x${ncpus}) on port ${PORT}`);
    for (let i = 0; i < ncpus; i++) {
      cluster.fork();
    }
  } else {
    const server = http.createServer((req, res) => {
      res.writeHead(200, {
        'Content-Length': '0',
        'Connection': 'close'
      });
      res.end();
    });
    server.listen(PORT);
  }

} else {
  console.error('Usage: node node-server.js [minimal|echo|cluster]');
  process.exit(1);
}
