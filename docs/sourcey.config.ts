export default {
  name: "icey",
  theme: {
    colors: { primary: "#2563eb" },
  },
  navigation: {
    tabs: [
      {
        tab: "Documentation",
        groups: [
          {
            group: "Start Here",
            pages: [
              "index",
              "getting-started",
              "installation",
              "installation-linux",
              "installation-osx",
              "installation-windows",
            ],
          },
          {
            group: "Core Concepts",
            pages: [
              "concepts/runtime-contracts",
              "concepts/packetstream",
              "concepts/http-lifecycle",
              "concepts/webrtc-session-flow",
            ],
          },
          {
            group: "Workflows",
            pages: [
              "recipes/http-server",
              "recipes/websocket-client-server",
              "recipes/turn-server",
              "recipes/webrtc-webcam-to-browser",
              "recipes/webrtc-browser-to-recorder",
              "recipes/media-server-stack",
            ],
          },
          {
            group: "Module Guides",
            pages: [
              "modules",
              "modules/base",
              "modules/crypto",
              "modules/net",
              "modules/http",
              "modules/json",
              "modules/av",
              "modules/webrtc",
              "modules/symple",
              "modules/stun",
              "modules/turn",
              "modules/archo",
              "modules/sched",
              "modules/pacm",
              "modules/pluga",
            ],
          },
          {
            group: "Project",
            pages: [
              "contributing",
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
