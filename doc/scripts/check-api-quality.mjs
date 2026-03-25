#!/usr/bin/env node

import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const here = dirname(fileURLToPath(import.meta.url));
const apiDir = join(here, '..', 'api');
const modules = [
  'archo.md',
  'av.md',
  'base.md',
  'crypto.md',
  'http.md',
  'json.md',
  'net.md',
  'pacm.md',
  'pluga.md',
  'sched.md',
  'stun.md',
  'symple.md',
  'turn.md',
  'uv.md',
  'webrtc.md',
];

const blankRow = /^\| .* \| .* \|\s*\|\s*$/;
const failures = [];

for (const name of modules) {
  const path = join(apiDir, name);
  const lines = readFileSync(path, 'utf8').split('\n');
  const overviewLimit = lines.findIndex((line) => line.trim() === '---');
  const overview = overviewLimit === -1 ? lines : lines.slice(0, overviewLimit);

  overview.forEach((line, index) => {
    if (blankRow.test(line)) {
      failures.push(`${name}:${index + 1}: ${line.trim()}`);
    }
  });
}

if (failures.length) {
  console.error('API markdown overview contains blank description rows:');
  for (const failure of failures) {
    console.error(`  ${failure}`);
  }
  process.exit(1);
}

console.log('API markdown overview quality check passed.');
