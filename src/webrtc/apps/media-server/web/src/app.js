import { SympleClient, Symple } from 'symple-client'
import { CallManager } from 'symple-client-player'

// ---------------------------------------------------------------------------
// DOM
// ---------------------------------------------------------------------------

const $status = document.getElementById('status')
const $peerList = document.getElementById('peer-list')
const $remoteVideo = document.getElementById('remote-video')
const $localVideo = document.getElementById('local-video')
const $overlay = document.getElementById('player-overlay')
const $controls = document.getElementById('controls')
const $connInfo = document.getElementById('connection-info')
const $statCodec = document.getElementById('stat-codec')
const $statBitrate = document.getElementById('stat-bitrate')

const $btnMute = document.getElementById('btn-mute')
const $btnCamera = document.getElementById('btn-camera')
const $btnFullscreen = document.getElementById('btn-fullscreen')
const $btnHangup = document.getElementById('btn-hangup')

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

let client = null
let calls = null
let audioMuted = false
let videoMuted = false
let statsInterval = null

// ---------------------------------------------------------------------------
// Connection
// ---------------------------------------------------------------------------

function getWsUrl () {
  const proto = location.protocol === 'https:' ? 'wss:' : 'ws:'
  return `${proto}//${location.host}`
}

function connect () {
  const url = getWsUrl()
  $connInfo.textContent = `Connecting to ${url}...`

  const user = 'viewer-' + Math.random().toString(36).slice(2, 6)

  client = new SympleClient({
    url,
    token: '',
    peer: {
      user,
      name: 'Browser Viewer',
      type: 'viewer'
    }
  })

  client.on('connect', () => {
    setOnline(true)
    $connInfo.textContent = `${url}  |  ${user}`
    updatePeerList()
  })

  client.on('presence', (p) => {
    updatePeerList()
  })

  client.on('addPeer', () => {
    updatePeerList()
  })

  client.on('removePeer', () => {
    updatePeerList()
  })

  client.on('disconnect', () => {
    setOnline(false)
    $connInfo.textContent = 'Disconnected'
    $peerList.innerHTML = ''
  })

  // Set up call manager
  calls = new CallManager(client, $remoteVideo, {
    rtcConfig: {
      iceServers: [
        { urls: 'stun:stun.l.google.com:19302' }
      ]
    },
    mediaConstraints: {
      audio: true,
      video: true
    },
    // Receive-only when calling the media server (it sends, we watch)
    localMedia: true
  })

  calls.on('incoming', (peerId, msg) => {
    // Auto-accept incoming calls from the media server
    console.log('Incoming call from', peerId)
    calls.accept()
  })

  calls.on('active', (peerId) => {
    console.log('Call active with', peerId)
    showControls(true)
    $overlay.classList.add('hidden')
    startStats()
  })

  calls.on('localstream', (stream) => {
    $localVideo.srcObject = stream
  })

  calls.on('ended', (peerId, reason) => {
    console.log('Call ended:', reason)
    showControls(false)
    $overlay.classList.remove('hidden')
    $remoteVideo.srcObject = null
    $localVideo.srcObject = null
    stopStats()
    updatePeerList()
  })

  calls.on('error', (err) => {
    console.error('Call error:', err)
  })

  client.on('error', (err) => {
    console.error('Client error:', err)
  })

  client.connect()
}

// ---------------------------------------------------------------------------
// Peer list
// ---------------------------------------------------------------------------

function updatePeerList () {
  if (!client || !client.roster) return

  $peerList.innerHTML = ''
  const peers = client.roster.peers()

  for (const peer of peers) {
    // Skip self
    if (peer.id === client.peer?.id) continue

    const li = document.createElement('li')
    li.className = 'peer-item'

    const info = document.createElement('div')
    info.className = 'peer-info'

    const dot = document.createElement('span')
    dot.className = 'peer-dot'

    const nameEl = document.createElement('div')
    const name = document.createElement('span')
    name.className = 'peer-name'
    name.textContent = peer.name || peer.user || peer.id

    const type = document.createElement('span')
    type.className = 'peer-type'
    type.textContent = peer.type || ''

    nameEl.appendChild(name)
    if (peer.type) {
      nameEl.appendChild(document.createElement('br'))
      nameEl.appendChild(type)
    }

    info.appendChild(dot)
    info.appendChild(nameEl)

    const btn = document.createElement('button')
    const address = Symple.buildAddress(peer)
    const inCall = calls && calls.remotePeerId === address

    if (inCall) {
      btn.textContent = 'Hangup'
      btn.className = 'hangup'
      btn.onclick = () => calls.hangup()
    } else {
      btn.textContent = 'Call'
      btn.onclick = () => {
        console.log('Calling', address)
        calls.call(address)
      }
    }

    li.appendChild(info)
    li.appendChild(btn)
    $peerList.appendChild(li)
  }

  if (peers.length === 0 || (peers.length === 1 && peers[0].id === client.peer?.id)) {
    const li = document.createElement('li')
    li.className = 'peer-item'
    li.innerHTML = '<span class="peer-type">No peers online</span>'
    $peerList.appendChild(li)
  }
}

// ---------------------------------------------------------------------------
// Controls
// ---------------------------------------------------------------------------

function showControls (visible) {
  $controls.classList.toggle('hidden', !visible)
}

$btnMute.addEventListener('click', () => {
  audioMuted = !audioMuted
  calls?.muteAudio(audioMuted)
  $btnMute.textContent = audioMuted ? '🔊' : '🔇'
})

$btnCamera.addEventListener('click', () => {
  videoMuted = !videoMuted
  calls?.muteVideo(videoMuted)
  $btnCamera.style.opacity = videoMuted ? '0.5' : '1'
})

$btnFullscreen.addEventListener('click', () => {
  const container = document.getElementById('player-container')
  if (document.fullscreenElement) {
    document.exitFullscreen()
  } else {
    container.requestFullscreen()
  }
})

$btnHangup.addEventListener('click', () => {
  calls?.hangup()
})

// ---------------------------------------------------------------------------
// Stats
// ---------------------------------------------------------------------------

function startStats () {
  stopStats()
  statsInterval = setInterval(async () => {
    if (!calls?.player?.pc) return

    try {
      const stats = await calls.player.pc.getStats()
      for (const report of stats.values()) {
        if (report.type === 'inbound-rtp' && report.kind === 'video') {
          const codec = report.decoderImplementation || ''
          const width = report.frameWidth
          const height = report.frameHeight
          $statCodec.textContent = codec && width ? `${codec} ${width}x${height}` : ''
        }
        if (report.type === 'candidate-pair' && report.state === 'succeeded') {
          const bitrate = report.availableIncomingBitrate
          if (bitrate) {
            $statBitrate.textContent = `${(bitrate / 1e6).toFixed(1)} Mbps`
          }
        }
      }
    } catch (_) { /* stats may fail during teardown */ }
  }, 2000)
}

function stopStats () {
  if (statsInterval) {
    clearInterval(statsInterval)
    statsInterval = null
  }
  $statCodec.textContent = ''
  $statBitrate.textContent = ''
}

// ---------------------------------------------------------------------------
// UI helpers
// ---------------------------------------------------------------------------

function setOnline (online) {
  $status.textContent = online ? 'Online' : 'Offline'
  $status.className = 'status ' + (online ? 'online' : 'offline')
}

// ---------------------------------------------------------------------------
// Boot
// ---------------------------------------------------------------------------

connect()
