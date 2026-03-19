import { defineConfig } from 'vite'

export default defineConfig({
  root: '.',
  build: {
    outDir: 'dist',
    emptyOutDir: true,
  },
  server: {
    // Proxy API requests to the C++ media server during dev.
    // WebSocket connections go directly to the C++ server (port 4500)
    // since the SympleClient URL points there explicitly.
    proxy: {
      '/api': {
        target: 'http://localhost:4500',
      },
    },
  },
})
