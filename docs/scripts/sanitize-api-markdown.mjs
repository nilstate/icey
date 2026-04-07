import { readdir, readFile, writeFile } from "node:fs/promises";
import { resolve, join } from "node:path";

function rewriteIncludePath(includePath) {
  const normalized = includePath.replace(/\\/g, "/");
  if (!normalized.startsWith("/") && !/^[A-Za-z]:\//.test(normalized)) {
    return includePath;
  }

  const includeMarker = "/include/";
  const includeIndex = normalized.lastIndexOf(includeMarker);
  if (includeIndex !== -1) {
    return normalized.slice(includeIndex + includeMarker.length);
  }

  const srcMarker = "/src/";
  const srcIndex = normalized.indexOf(srcMarker);
  if (srcIndex !== -1) {
    return normalized.slice(srcIndex + 1);
  }

  return normalized.split("/").pop() ?? includePath;
}

function sanitizeMarkdown(markdown) {
  return markdown.replace(/^#include ([<"])([^>\n"]+)([>"])$/gm, (_match, open, includePath, close) => {
    return `#include ${open}${rewriteIncludePath(includePath)}${close}`;
  });
}

async function main() {
  const apiDir = resolve(process.argv[2] ?? "docs/api");
  const entries = await readdir(apiDir, { withFileTypes: true });

  for (const entry of entries) {
    if (!entry.isFile() || !entry.name.endsWith(".md")) continue;

    const file = join(apiDir, entry.name);
    const original = await readFile(file, "utf-8");
    const sanitized = sanitizeMarkdown(original);

    if (sanitized !== original) {
      await writeFile(file, sanitized, "utf-8");
    }
  }
}

await main();
