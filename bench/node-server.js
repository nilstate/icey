// Node.js HTTP server for benchmark comparison
const http = require('http');
const cluster = require('cluster');
const os = require('os');

const PORT = 1337;
const mode = process.argv[2] || 'single';

if (mode === 'cluster' && cluster.isPrimary) {
  const ncpus = os.cpus().length;
  for (let i = 0; i < ncpus; i++) cluster.fork();
  console.log(`Node.js HTTP benchmark (cluster x${ncpus}) listening on port ${PORT}`);
} else {
  const server = http.createServer((req, res) => {
    res.writeHead(200, {
      'Content-Length': '0',
      'Connection': 'close'
    });
    res.end();
  });

  server.listen(PORT, '0.0.0.0', () => {
    if (mode !== 'cluster' || !cluster.isPrimary) {
      if (mode === 'single') {
        console.log(`Node.js HTTP benchmark (single-core) listening on port ${PORT}`);
      }
    }
  });
}
