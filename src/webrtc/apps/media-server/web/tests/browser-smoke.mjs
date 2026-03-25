import { spawn } from 'node:child_process'
import { access, mkdtemp, readdir, rm, stat } from 'node:fs/promises'
import { constants } from 'node:fs'
import os from 'node:os'
import path from 'node:path'
import process from 'node:process'
import { setTimeout as delay } from 'node:timers/promises'

import { chromium, firefox, webkit } from 'playwright'


const __dirname = import.meta.dirname
const repoRoot = path.resolve(__dirname, '../../../../../..')


function candidatePath(...segments) {
  return path.resolve(repoRoot, ...segments)
}

function fail(message) {
  throw new Error(message)
}

async function exists(filePath) {
  try {
    await access(filePath, constants.F_OK)
    return true
  } catch {
    return false
  }
}

async function isUsableExecutable(filePath) {
  try {
    await access(filePath, constants.X_OK)
    const info = await stat(filePath)
    return info.isFile() && info.size > 0
  } catch {
    return false
  }
}

async function findFirstExisting(paths, label, predicate = exists) {
  for (const filePath of paths) {
    if (await predicate(filePath))
      return filePath
  }
  fail(`${label} not found. Tried:\n${paths.map((p) => `- ${p}`).join('\n')}`)
}

async function waitForHttp(url, timeoutMs) {
  const deadline = Date.now() + timeoutMs
  let lastError = null
  while (Date.now() < deadline) {
    try {
      const response = await fetch(url)
      if (response.ok || response.status === 404)
        return
    } catch (err) {
      lastError = err
    }
    await delay(250)
  }
  throw new Error(`Timed out waiting for ${url}: ${lastError ?? 'server never responded'}`)
}

function selectedBrowserEngine() {
  return (process.env.MEDIA_SERVER_BROWSER_ENGINE || 'chromium').toLowerCase()
}

async function findBrowserExecutable(engine) {
  const explicitExecutable = {
    chromium: process.env.MEDIA_SERVER_CHROMIUM_BROWSER,
    firefox: process.env.MEDIA_SERVER_FIREFOX_BROWSER,
    webkit: process.env.MEDIA_SERVER_WEBKIT_BROWSER
  }[engine] || process.env.MEDIA_SERVER_BROWSER

  if (!explicitExecutable)
    return null
  if (!await exists(explicitExecutable))
    fail(`${engine} executable not found: ${explicitExecutable}`)
  return explicitExecutable
}

async function findMediaServerBinary() {
  return findFirstExisting([
    process.env.MEDIA_SERVER_BIN,
    candidatePath('build-webrtc/webrtc/apps/media-server/media-server'),
    candidatePath('build-webrtc-samples/webrtc/apps/media-server/media-server'),
    candidatePath('build/bin/media-server')
  ].filter(Boolean), 'media-server binary', isUsableExecutable)
}

async function withServer(args, run) {
  const serverBin = await findMediaServerBinary()
  const child = spawn(serverBin, args, {
    cwd: repoRoot,
    stdio: ['ignore', 'pipe', 'pipe']
  })

  const logs = []
  const capture = (stream, label) => {
    stream.on('data', (chunk) => {
      for (const line of chunk.toString().split(/\r?\n/)) {
        if (!line)
          continue
        logs.push(`[${label}] ${line}`)
        if (logs.length > 200)
          logs.shift()
      }
    })
  }

  capture(child.stdout, 'out')
  capture(child.stderr, 'err')

  try {
    const portIndex = args.indexOf('--port')
    const port = portIndex !== -1 ? args[portIndex + 1] : '4500'
    await waitForHttp(`http://127.0.0.1:${port}/`, 15000)
    return await run({port, logs})
  } catch (err) {
    const context = logs.length ? `\nLast server logs:\n${logs.join('\n')}` : ''
    throw new Error(`${err.message}${context}`)
  } finally {
    child.kill('SIGTERM')
    await Promise.race([
      new Promise((resolve) => child.once('exit', resolve)),
      delay(5000).then(() => child.kill('SIGKILL'))
    ])
  }
}

async function launchBrowser() {
  const engine = selectedBrowserEngine()
  const executablePath = await findBrowserExecutable(engine)
  const launchOptions = { headless: true }

  if (engine === 'chromium') {
    launchOptions.args = [
      '--use-fake-ui-for-media-stream',
      '--use-fake-device-for-media-stream',
      '--autoplay-policy=no-user-gesture-required',
      '--no-first-run',
      '--no-default-browser-check'
    ]
    if (executablePath)
      launchOptions.executablePath = executablePath
    return chromium.launch(launchOptions)
  }

  if (engine === 'firefox') {
    launchOptions.firefoxUserPrefs = {
      'media.navigator.streams.fake': true,
      'media.navigator.permission.disabled': true,
      'media.autoplay.default': 0,
      'media.autoplay.blocking_policy': 0
    }
    if (executablePath)
      launchOptions.executablePath = executablePath
    return firefox.launch(launchOptions)
  }

  if (engine === 'webkit') {
    if (executablePath)
      launchOptions.executablePath = executablePath
    return webkit.launch(launchOptions)
  }

  fail(`Unsupported browser engine '${engine}'`)
}

async function newPage(browser, baseUrl, label) {
  const engine = selectedBrowserEngine()
  const contextOptions = {}
  if (!['firefox', 'webkit'].includes(engine))
    contextOptions.permissions = ['camera', 'microphone']

  const context = await browser.newContext(contextOptions)
  const page = await context.newPage()
  page.on('console', (msg) => {
    if (msg.type() === 'error')
      console.error(`[browser:${label}] ${msg.text()}`)
  })
  page.on('pageerror', (err) => {
    console.error(`[page:${label}] ${err.message}`)
  })
  await page.goto(baseUrl, { waitUntil: 'domcontentloaded' })
  await page.waitForFunction(() => {
    const status = document.getElementById('status')
    return status?.classList.contains('online')
  }, null, { timeout: 15000 })
  await page.evaluate(() => {
    const state = globalThis.__mediaServerState
    const client = state?.client
    if (!state || !client || state._sendWrapped) {
      return
    }
    const sent = []
    const originalSend = client.send.bind(client)
    client.send = (message, to) => {
      sent.push({
        subtype: message?.subtype ?? null,
        to: to ?? message?.to ?? null,
        dataType: message?.data?.type ?? null,
        dataSdpLength: typeof message?.data?.sdp === 'string' ? message.data.sdp.length : 0
      })
      if (sent.length > 20) {
        sent.shift()
      }
      return originalSend(message, to)
    }
    state.sentMessages = sent
    state.receivedMessages = []
    client.on('message', (message) => {
      const received = state.receivedMessages
      if (!Array.isArray(received)) {
        return
      }
      const candidate = typeof message?.data?.candidate === 'string'
        ? message.data.candidate
        : null
      received.push({
        subtype: message?.subtype ?? null,
        from: typeof message?.from === 'string'
          ? message.from
          : (message?.from?.id || message?.from?.user || null),
        dataType: message?.data?.type ?? null,
        dataSdpLength: typeof message?.data?.sdp === 'string' ? message.data.sdp.length : 0,
        hasCandidate: candidate !== null,
        candidate: candidate ? candidate.slice(0, 160) : null
      })
      if (received.length > 30) {
        received.shift()
      }
    })
    state._sendWrapped = true
  })
  return {context, page}
}

async function callPeer(page, peerName = 'Media Server', actionText = 'Call') {
  const item = page.locator('#peer-list li', { hasText: peerName }).first()
  await item.waitFor({ state: 'visible', timeout: 15000 })
  const button = item.locator('button', { hasText: actionText })
  await button.waitFor({ state: 'visible', timeout: 15000 })
  await button.click()
}

async function waitForActive(page) {
  try {
    await page.waitForFunction(() => {
      const controls = document.getElementById('controls')
      return controls && !controls.classList.contains('hidden')
    }, null, { timeout: 20000 })
  } catch (err) {
    const state = await capturePlaybackState(page)
    err.message += `\nCall state:\n${JSON.stringify(state, null, 2)}`
    throw err
  }
}

async function capturePlaybackState(page) {
  return page.evaluate(async () => {
    const remote = document.getElementById('remote-video')
    const local = document.getElementById('local-video')
    const codec = document.getElementById('stat-codec')
    const bitrate = document.getElementById('stat-bitrate')

    const calls = globalThis.__mediaServerState?.calls || null
    const player = calls?.player || null
    const pc = player?.pc || null

      const stats = []
      if (pc?.getStats) {
        const reports = await pc.getStats()
        for (const report of reports.values()) {
          if (report.type === 'inbound-rtp' ||
            report.type === 'outbound-rtp' ||
            report.type === 'remote-inbound-rtp' ||
            report.type === 'candidate-pair' ||
            report.type === 'codec' ||
            report.type === 'track') {
          stats.push(report)
        }
      }
    }

    return {
      controlsHidden: document.getElementById('controls')?.classList.contains('hidden'),
      overlayHidden: document.getElementById('player-overlay')?.classList.contains('hidden'),
      codecText: codec?.textContent?.trim() || '',
      bitrateText: bitrate?.textContent?.trim() || '',
      remote: remote ? {
        srcObject: Boolean(remote.srcObject),
        readyState: remote.readyState,
        currentTime: remote.currentTime,
        paused: remote.paused,
        ended: remote.ended,
        muted: remote.muted,
        autoplay: remote.autoplay,
        playsInline: remote.playsInline,
        tracks: remote.srcObject
          ? remote.srcObject.getTracks().map((track) => ({
              kind: track.kind,
              id: track.id,
              enabled: track.enabled,
              muted: track.muted,
              readyState: track.readyState
            }))
          : []
      } : null,
      local: local ? {
        srcObject: Boolean(local.srcObject),
        readyState: local.readyState,
        currentTime: local.currentTime,
        paused: local.paused,
        ended: local.ended,
        muted: local.muted,
        autoplay: local.autoplay,
        playsInline: local.playsInline,
        tracks: local.srcObject
          ? local.srcObject.getTracks().map((track) => ({
              kind: track.kind,
              id: track.id,
              enabled: track.enabled,
              muted: track.muted,
              readyState: track.readyState
            }))
          : []
      } : null,
      peerConnection: pc ? {
        signalingState: pc.signalingState,
        connectionState: pc.connectionState,
        iceConnectionState: pc.iceConnectionState,
        localDescriptionType: pc.localDescription?.type ?? null,
        localDescriptionSdp: typeof pc.localDescription?.sdp === 'string'
          ? pc.localDescription.sdp
          : null,
        remoteDescriptionType: pc.remoteDescription?.type ?? null,
        remoteDescriptionSdp: typeof pc.remoteDescription?.sdp === 'string'
          ? pc.remoteDescription.sdp
          : null,
        transceivers: typeof pc.getTransceivers === 'function'
          ? pc.getTransceivers().map((transceiver) => ({
              mid: transceiver.mid ?? null,
              direction: transceiver.direction ?? null,
              currentDirection: transceiver.currentDirection ?? null,
              senderTrackKind: transceiver.sender?.track?.kind ?? null,
              receiverTrackKind: transceiver.receiver?.track?.kind ?? null
            }))
          : []
      } : null,
      sentMessages: Array.isArray(globalThis.__mediaServerState?.sentMessages)
        ? globalThis.__mediaServerState.sentMessages.slice()
        : [],
      receivedMessages: Array.isArray(globalThis.__mediaServerState?.receivedMessages)
        ? globalThis.__mediaServerState.receivedMessages.slice()
        : [],
      stats
    }
  })
}

async function waitForInboundVideo(page) {
  try {
    await page.waitForFunction(() => {
      const video = document.getElementById('remote-video')
      const codec = document.getElementById('stat-codec')
      if (!video || !codec)
        return false
      return Boolean(video.srcObject) &&
             (video.readyState >= 2 || video.currentTime > 0 || codec.textContent.trim().length > 0)
    }, null, { timeout: 20000 })
  } catch (err) {
    const state = await capturePlaybackState(page)
    err.message += `\nPlayback state:\n${JSON.stringify(state, null, 2)}`
    throw err
  }
}

async function waitForRecording(recordDir) {
  const deadline = Date.now() + 20000
  while (Date.now() < deadline) {
    const names = await readdir(recordDir)
    for (const name of names) {
      if (!name.endsWith('.mp4'))
        continue
      const info = await stat(path.join(recordDir, name))
      if (info.size > 0)
        return
    }
    await delay(500)
  }
  fail(`No non-empty MP4 recording appeared in ${recordDir}`)
}

async function runStreamScenario(browser, webRoot, sourceFile) {
  console.log('browser smoke: stream mode')
  await withServer([
    '--port', '4600',
    '--mode', 'stream',
    '--web-root', webRoot,
    '--source', sourceFile
  ], async ({port}) => {
    const {context, page} = await newPage(browser, `http://127.0.0.1:${port}/`, 'stream')
    try {
      await callPeer(page, 'Media Server', 'Watch')
      await waitForActive(page)
      await waitForInboundVideo(page)
    } finally {
      await context.close()
    }
  })
}

async function runRecordScenario(browser, webRoot) {
  console.log('browser smoke: record mode')
  const recordDir = await mkdtemp(path.join(os.tmpdir(), 'icey-record-'))
  try {
    await withServer([
      '--port', '4601',
      '--mode', 'record',
      '--web-root', webRoot,
      '--record-dir', recordDir
    ], async ({port}) => {
      const {context, page} = await newPage(browser, `http://127.0.0.1:${port}/`, 'record')
      try {
        await callPeer(page, 'Media Server', 'Broadcast')
        await waitForActive(page)
        try {
          await waitForRecording(recordDir)
        } catch (err) {
          const state = await capturePlaybackState(page)
          err.message += `\nRecord state:\n${JSON.stringify(state, null, 2)}`
          throw err
        }
      } finally {
        await context.close()
      }

      try {
        await waitForRecording(recordDir)
      } catch (err) {
        const recordings = await readdir(recordDir)
        err.message += `\nRecording files:\n${JSON.stringify(recordings, null, 2)}`
        throw err
      }
    })
  } finally {
    await rm(recordDir, { recursive: true, force: true })
  }
}

async function runRelayScenario(browser, webRoot) {
  console.log('browser smoke: relay mode')
  await withServer([
    '--port', '4602',
    '--mode', 'relay',
    '--web-root', webRoot
  ], async ({port}) => {
    const source = await newPage(browser, `http://127.0.0.1:${port}/`, 'relay-source')
    const viewer = await newPage(browser, `http://127.0.0.1:${port}/`, 'relay-viewer')
    try {
      await callPeer(source.page, 'Media Server', 'Broadcast')
      await waitForActive(source.page)

      await callPeer(viewer.page, 'Media Server', 'Watch')
      await waitForActive(viewer.page)
      await waitForInboundVideo(viewer.page)
    } finally {
      await Promise.all([
        source.context.close(),
        viewer.context.close()
      ])
    }
  })
}

async function main() {
  const webRoot = path.resolve(__dirname, '../dist')
  const sourceFile = candidatePath('data/test.mp4')
  const engine = selectedBrowserEngine()

  if (!await exists(webRoot))
    fail(`Built web UI not found at ${webRoot}. Run 'npm run build' in src/webrtc/apps/media-server/web first.`)
  if (!await exists(path.join(webRoot, 'index.html')))
    fail(`Expected ${path.join(webRoot, 'index.html')} to exist.`)
  if (!await exists(sourceFile))
    fail(`Media source not found at ${sourceFile}.`)

  const browser = await launchBrowser()
  try {
    console.log(`browser smoke engine: ${engine}`)
    await runStreamScenario(browser, webRoot, sourceFile)
    await runRecordScenario(browser, webRoot)
    await runRelayScenario(browser, webRoot)
  } finally {
    await browser.close()
  }

  console.log('browser smoke: all scenarios passed')
}

main().catch((err) => {
  console.error(err.stack || err.message)
  process.exit(1)
})
