# Pacm

> Embeddable C++ package manager for native applications

**[API Reference →](../api/pacm.md)** · **[Source →](https://github.com/nilstate/icey/tree/main/src/pacm/)**

## Overview

The `pacm` module is an embeddable package manager. We use it to give native applications the ability to install, update, and uninstall versioned archives (plugins, assets, or any packaged payload) from a remote HTTP endpoint, without shipping a separate management tool.

The server side is a plain JSON index served over HTTP; no special server software is required. The client fetches the index, compares it against locally-stored manifests, and downloads only what has changed. Downloads are verified by checksum before extraction. Installation is a three-phase pipeline: download, extract (via `archo`), finalize (move files into place).

**Dependencies:** `base`, `net`, `json`, `http`, `archo`, `crypto`

## Architecture

Five classes carry the module's responsibilities:

- `PackageManager` is the central coordinator. It owns the local and remote package stores, creates `InstallTask` instances on demand, and emits signals at key lifecycle points.
- `Package` / `RemotePackage` / `LocalPackage` are JSON-backed value types describing packages on the server and on disk.
- `InstallTask` runs one installation pipeline asynchronously: download → extract → finalize. It is a state machine and emits progress and completion signals.
- `InstallMonitor` coordinates multiple concurrent `InstallTask` instances and aggregates their progress into a single overall signal.
- `InstallOptions` carries per-operation overrides (version pin, SDK version pin, install directory override).

```text
PackageManager
  ├── RemotePackageStore  (LiveCollection<id, RemotePackage>)
  ├── LocalPackageStore   (LiveCollection<id, LocalPackage>)
  └── InstallTask  [0..*]
        ├── download  (http)
        ├── extract   (archo)
        └── finalize  (filesystem move)

InstallMonitor  (optional; aggregates N InstallTasks)
```

## Usage

### Basic setup

Construct a `PackageManager` with an `Options` struct, then call `initialize()`. All directories default to subdirectories of the current working directory (`pacm/install`, `pacm/data`, `pacm/tmp`) if not overridden.

```cpp
#include <icy/pacm/packagemanager.h>

icy::pacm::PackageManager::Options opts;
opts.endpoint   = "https://packages.example.com";
opts.indexURI   = "/packages.json";
opts.installDir = "/opt/myapp/plugins";
opts.dataDir    = "/var/myapp/pacm/data";
opts.tempDir    = "/tmp/myapp/pacm";

icy::pacm::PackageManager pm(opts);
pm.initialize();
```

`initialize()` creates the directory structure and loads any previously saved local manifests from `dataDir`.

### Querying the remote package list

```cpp
pm.queryRemotePackages();

for (auto& [id, pkg] : pm.remotePackages().map())
    std::cout << "remote: " << id << "  " << pkg->latestAsset().version() << "\n";

for (auto& [id, pkg] : pm.localPackages().map())
    std::cout << "local:  " << id << "  " << pkg->version() << "\n";
```

`queryRemotePackages()` fires the `RemotePackageResponse` signal when the HTTP response arrives.

### Installing a single package

`installPackage()` returns a started-but-not-yet-running `InstallTask::Ptr`. Attach signal handlers before calling `start()`. The method returns `nullptr` if the package is already up to date.

```cpp
auto task = pm.installPackage("myplugin");
if (task) {
    task->Progress += [](icy::pacm::InstallTask& t, int& pct) {
        std::cout << "progress: " << pct << "%\n";
    };
    task->Complete += [](icy::pacm::InstallTask& t) {
        if (t.success())
            std::cout << "installed\n";
        else
            std::cout << "failed\n";
    };
    task->start();
}
```

### Installing multiple packages with a monitor

Pass an `InstallMonitor` to `installPackages()` to hold all tasks unstarted until after signal handlers are attached, then call `startAll()`.

```cpp
icy::pacm::InstallMonitor monitor;

monitor.Progress += [](int& pct) {
    std::cout << "overall: " << pct << "%\n";
};
monitor.Complete += [](icy::pacm::LocalPackageVec& pkgs) {
    std::cout << "all done: "
              << icy::pacm::getInstallTaskNamesString(pkgs) << "\n";
};

std::vector<std::string> ids = { "myplugin", "anotherpackage" };
pm.installPackages(ids, {}, &monitor);
monitor.startAll();
```

Without a monitor, `installPackages()` starts all tasks immediately.

### Version and SDK pinning

```cpp
icy::pacm::InstallOptions opts;

// Pin to an exact package version.
opts.version = "1.2.0";
pm.installPackage("myplugin", opts);

// Install the latest build compatible with a specific SDK version.
opts.version    = "";
opts.sdkVersion = "3.0.0";
pm.installPackage("myplugin", opts);

// Override the install directory for this operation only.
opts.installDir = "/opt/myapp/experimental";
pm.installPackage("myplugin", opts);
```

SDK version matching is intended for plugins that must be compiled against a specific SDK release. The package JSON must include a `"sdk-version"` field on each asset for this to work.

### Version locking on a local package

We can prevent a local package from being updated past a given version:

```cpp
auto& locals = pm.localPackages().map();
auto it = locals.find("myplugin");
if (it != locals.end()) {
    it->second->setVersionLock("1.2.0");   // never update past 1.2.0
    pm.saveLocalPackages();
}
```

Pass an empty string to `setVersionLock()` to remove the lock. `setSDKVersionLock()` works the same way for SDK-version constraints.

### Updating packages

```cpp
// Update a single package.
auto task = pm.updatePackage("myplugin");
if (task) task->start();

// Update all installed packages.
pm.updateAllPackages();
```

`updatePackage()` throws if the package is not known. `updateAllPackages()` silently skips packages that are already current.

### Uninstalling packages

```cpp
pm.uninstallPackage("myplugin");
pm.uninstallPackages({ "myplugin", "anotherpackage" });
```

The `PackageUninstalled` signal fires for each removed package.

### Handling unfinalized installations

If target files are in use at extraction time, installed files are left in a staging area with the state `Finalizing`. On the next startup we check for these and complete the move:

```cpp
if (pm.hasUnfinalizedPackages())
    pm.finalizeInstallations();
```

### Inspecting package pairs

`PackagePair` binds a `LocalPackage*` and a `RemotePackage*` together; either pointer may be null if the package exists on only one side.

```cpp
for (auto& pair : pm.getPackagePairs()) {
    std::cout << pair.id() << "\n";
    if (pair.local)  std::cout << "  installed: " << pair.local->version()  << "\n";
    if (pair.remote) std::cout << "  available: " << pair.remote->latestAsset().version() << "\n";
}

// Only pairs with a newer remote version available.
for (auto& pair : pm.getUpdatablePackagePairs())
    std::cout << "update available: " << pair.id() << "\n";
```

### Cancelling all tasks

```cpp
pm.cancelAllTasks();
```

All in-progress downloads and extractions are aborted. Call this before destroying the `PackageManager` if tasks may still be running.

### Server Protocol

The server responds to `GET /packages.json` with a JSON array. Each element describes one package and its available assets:

```json
[{
    "id": "myplugin",
    "type": "Plugin",
    "name": "My Plugin",
    "author": "Example Corp",
    "description": "An example plugin.",
    "assets": [{
        "version": "1.0.0",
        "sdk-version": "2.0.0",
        "platform": "linux",
        "checksum": "e4d909c290d0fb1ca068ffaddf22cbd0",
        "file-name": "myplugin-1.0.0-sdk-2.0.0-linux.zip",
        "file-size": 432321,
        "mirrors": [
            { "url": "https://packages.example.com/myplugin-1.0.0-sdk-2.0.0-linux.zip" }
        ]
    }]
}]
```

The `platform` field on each asset is matched against `Options::platform`. The `mirrors` array may contain multiple download URLs; the first is used. Additional metadata fields may be added freely; the client ignores anything it does not recognise.

`InstallationState` transitions during a typical install:

```text
None → Downloading → Extracting → Finalizing → Installed
                                              → Failed
                  → Cancelled
```

## Samples

### [pacm-cli](https://github.com/nilstate/icey/tree/main/src/pacm/apps/pacm-cli/)

The redistributable command-line tool bundled with icey. It wraps the full `PackageManager` API and can be compiled and shipped alongside existing applications as a standalone package management utility.

```bash
# Install a package.
pacm-cli -endpoint https://packages.example.com -uri /packages.json -install myplugin

# Update all installed packages.
pacm-cli -endpoint https://packages.example.com -uri /packages.json -update

# Uninstall a package.
pacm-cli -endpoint https://packages.example.com -uri /packages.json -uninstall myplugin
```

Supported flags:

| Flag | Description |
| ---- | ----------- |
| `-endpoint` | HTTP server base URL. |
| `-uri` | Path to the package JSON index. |
| `-install` | Comma-separated package IDs to install. |
| `-uninstall` | Comma-separated package IDs to uninstall. |
| `-update` | Update all installed packages. |
| `-print` | Print all local and remote packages on exit. |
| `-install-dir` | Override the installation directory. |
| `-data-dir` | Override the manifest storage directory. |
| `-temp-dir` | Override the download staging directory. |
| `-checksum-alg` | Checksum algorithm (default: `SHA256`). |
| `-logfile` | Log output path. |

## Configuration

`PackageManager::Options` controls all configurable behaviour:

| Field | Default | Description |
| ----- | ------- | ----------- |
| `endpoint` | `https://localhost:3000` | HTTP server base URL. |
| `indexURI` | `/packages.json` | Path to the package index on the server. |
| `httpUsername` | `""` | HTTP Basic Auth username. |
| `httpPassword` | `""` | HTTP Basic Auth password. |
| `httpOAuthToken` | `""` | OAuth token; used instead of Basic Auth when set. |
| `installDir` | `pacm/install` | Final installation destination. |
| `dataDir` | `pacm/data` | Local manifest storage. |
| `tempDir` | `pacm/tmp` | Download and extraction staging area. |
| `platform` | auto-detected | Platform string matched against asset records (`linux`, `win32`, `mac`). |
| `checksumAlgorithm` | `SHA256` | Algorithm used to verify downloaded archives. |
| `clearFailedCache` | `true` | Whether to delete the cached archive if installation fails. |

All directory defaults are relative to the current working directory.

## See Also

- [Archo](archo.md) — ZIP extraction used internally by `InstallTask`
- [Pluga](pluga.md) — plugin loading; use alongside `pacm` for full remote plugin support
- [JSON](json.md) — JSON types and file I/O used throughout `pacm`
- [Base](base.md) — signals, tasks, collections, and filesystem helpers
