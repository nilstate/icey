export default {
  name: "Icey",
  theme: {
    colors: { primary: "#2563eb" },
  },
  navigation: {
    tabs: [
      {
        tab: "Documentation",
        groups: [
          {
            group: "Getting Started",
            pages: [
              "README",
              "getting-started",
              "installation",
            ],
          },
          {
            group: "Platform Guides",
            pages: [
              "installation-linux",
              "installation-osx",
              "installation-windows",
            ],
          },
          {
            group: "Core Modules",
            pages: [
              "modules/base",
              "modules/crypto",
              "modules/net",
              "modules/http",
              "modules/json",
            ],
          },
          {
            group: "Media & WebRTC",
            pages: [
              "modules/av",
              "modules/webrtc",
              "modules/symple",
              "modules/stun",
              "modules/turn",
            ],
          },
          {
            group: "Utilities",
            pages: [
              "modules/archo",
              "modules/sched",
              "modules/pacm",
              "modules/pluga",
            ],
          },
          {
            group: "Reference",
            pages: [
              "modules",
              "contributing",
            ],
          },
        ],
      },
      {
        tab: "C++ API",
        doxygen: {
          xml: "../build/doxygen/xml",
          language: "cpp",
        },
      },
    ],
  },
  navbar: {
    links: [
      { type: "github", href: "https://github.com/sourcey/icey" },
    ],
  },
};
