export default {
  name: "icey",
  logo: "./icey.png",
  repo: "https://github.com/nilstate/icey",
  editBranch: "main",
  editBasePath: "docs",
  theme: {
    colors: { primary: "#BD1E1E" },
  },
  navigation: {
    tabs: [
      {
        tab: "Guides",
        groups: [
          {
            group: "Run",
            pages: [
              "index",
              "run/index",
              "run/install",
              "run/modes",
              "run/cli",
            ],
          },
          {
            group: "Build",
            pages: [
              "build/getting-started",
              "build/installation",
              "recipes/http-server",
              "recipes/websocket-client-server",
              "recipes/turn-server",
              "recipes/webrtc-webcam-to-browser",
              "recipes/webrtc-browser-to-recorder",
            ],
          },
          {
            group: "Operate",
            pages: [
              "operate/config",
              "operate/deploy",
              "operate/tls",
              "operate/turn",
              "operate/health",
              "operate/troubleshoot",
            ],
          },
        ],
      },
      {
        tab: "Concepts",
        groups: [
          {
            group: "Understand",
            pages: [
              "concepts/architecture",
              "modules",
              "concepts/runtime-contracts",
              "concepts/packetstream",
              "concepts/http-lifecycle",
              "concepts/webrtc-session-flow",
            ],
          },
          {
            group: "Module Guides",
            pages: [
              "modules/base",
              "modules/crypto",
              "modules/net",
              "modules/http",
              "modules/json",
              "modules/av",
              "modules/speech",
              "modules/vision",
              "modules/webrtc",
              "modules/symple",
              "modules/stun",
              "modules/turn",
              "modules/archo",
              "modules/graft",
              "modules/pacm",
              "modules/sched",
            ],
          },
          {
            group: "Project",
            pages: [
              "contributing",
              "conventions",
              "releasing",
            ],
          },
        ],
      },
      {
        tab: "API Reference",
        doxygen: {
          xml: "../build/doxygen/xml",
          language: "cpp",
          index: "rich",
        },
      },
    ],
  },
  navbar: {
    links: [
      { type: "github", href: "https://github.com/nilstate/icey" },
    ],
  },
};
