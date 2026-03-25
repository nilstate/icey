{#pacmmodule}

# pacm

Package manager for distributing and installing plugins.

### Namespaces

| Name | Description |
|------|-------------|
| [`pacm`](#pacm) | [Package](#package) manifests, install tasks, and repository management helpers. |

{#pacm}

# pacm

[Package](#package) manifests, install tasks, and repository management helpers.

### Classes

| Name | Description |
|------|-------------|
| [`InstallMonitor`](#installmonitor) | Aggregates multiple install tasks and reports overall progress. |
| [`InstallTask`](#installtask) | Downloads, extracts, and finalizes a single package installation. |
| [`PackageManager`](#packagemanager) | Loads package manifests and coordinates install, update, and uninstall workflows. |
| [`InstallationState`](#installationstate) | [State](base.md#state) machine states for package installation. |
| [`InstallOptions`](#installoptions) | [Package](#package) installation options. |
| [`LocalPackage`](#localpackage) | [Package](#package) metadata for an installed package on the local filesystem. |
| [`Package`](#package) | JSON-backed package metadata shared by local and remote package records. |
| [`PackagePair`](#packagepair) | Pairing of the installed and remote metadata for the same package ID. |
| [`RemotePackage`](#remotepackage) | [Package](#package) metadata loaded from the remote package index. |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< LocalPackage * >` | [`LocalPackageVec`](#localpackagevec)  | Vector of local package pointers used by install monitor progress snapshots. |
| `std::vector< InstallTask * >` | [`InstallTaskVec`](#installtaskvec)  | Vector of raw install task pointers used for transient iteration. |
| `std::vector< InstallTask::Ptr >` | [`InstallTaskPtrVec`](#installtaskptrvec)  | Vector of shared install task handles retained across async workflows. |
| `std::vector< PackagePair >` | [`PackagePairVec`](#packagepairvec)  | Vector of local/remote package pairs used for reconciliation and update checks. |
| `KeyedStore< std::string, LocalPackage >` | [`LocalPackageStore`](#localpackagestore)  | Keyed store of installed packages indexed by package ID. |
| `KeyedStore< std::string, RemotePackage >` | [`RemotePackageStore`](#remotepackagestore)  | Keyed store of remote package metadata indexed by package ID. |

---

{#localpackagevec}

#### LocalPackageVec

```cpp
std::vector< LocalPackage * > LocalPackageVec()
```

Vector of local package pointers used by install monitor progress snapshots.

---

{#installtaskvec}

#### InstallTaskVec

```cpp
std::vector< InstallTask * > InstallTaskVec()
```

Vector of raw install task pointers used for transient iteration.

---

{#installtaskptrvec}

#### InstallTaskPtrVec

```cpp
std::vector< InstallTask::Ptr > InstallTaskPtrVec()
```

Vector of shared install task handles retained across async workflows.

---

{#packagepairvec}

#### PackagePairVec

```cpp
std::vector< PackagePair > PackagePairVec()
```

Vector of local/remote package pairs used for reconciliation and update checks.

---

{#localpackagestore}

#### LocalPackageStore

```cpp
KeyedStore< std::string, LocalPackage > LocalPackageStore()
```

Keyed store of installed packages indexed by package ID.

---

{#remotepackagestore}

#### RemotePackageStore

```cpp
KeyedStore< std::string, RemotePackage > RemotePackageStore()
```

Keyed store of remote package metadata indexed by package ID.

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`getInstallTaskNamesString`](#getinstalltasknamesstring) `inline` | Returns a comma-delimited string of display names from `packages`.  |
| `void` | [`validatePathComponent`](#validatepathcomponent) `inline` | Validates that a string is safe to use as a path component. Rejects path traversal sequences (..), directory separators (/ and ), null bytes, and empty strings.  |

---

{#getinstalltasknamesstring}

#### getInstallTaskNamesString

`inline`

```cpp
inline std::string getInstallTaskNamesString(LocalPackageVec & packages)
```

Returns a comma-delimited string of display names from `packages`. 
#### Parameters
* `packages` Vector of [LocalPackage](#localpackage) pointers to format. 

#### Returns
Comma-separated name string, e.g. "PluginA, PluginB".

---

{#validatepathcomponent}

#### validatePathComponent

`inline`

```cpp
inline void validatePathComponent(std::string_view name, std::string_view context)
```

Validates that a string is safe to use as a path component. Rejects path traversal sequences (..), directory separators (/ and ), null bytes, and empty strings. 
#### Parameters
* `name` The path component to validate. 

* `context` Caller description included in the exception message. 

#### Exceptions
* `std::invalid_argument` if `name` fails any validation check.

{#installmonitor}

## InstallMonitor

```cpp
#include <icy/pacm/installmonitor.h>
```

Aggregates multiple install tasks and reports overall progress.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `ThreadSignal< void(InstallTask &, const InstallationState &, const InstallationState &)>` | [`InstallStateChange`](#installstatechange)  | Proxies state change events from managed packages. |
| `ThreadSignal< void(LocalPackage &)>` | [`InstallComplete`](#installcomplete)  | Signals when a managed install task completes. |
| `ThreadSignal< void(int &)>` | [`Progress`](#progress-1)  | Signals on overall progress update [0-100]. |
| `ThreadSignal< void(LocalPackageVec &)>` | [`Complete`](#complete-3)  | Signals on all tasks complete. |

---

{#installstatechange}

#### InstallStateChange

```cpp
ThreadSignal< void(InstallTask &, const InstallationState &, const InstallationState &)> InstallStateChange
```

Proxies state change events from managed packages.

---

{#installcomplete}

#### InstallComplete

```cpp
ThreadSignal< void(LocalPackage &)> InstallComplete
```

Signals when a managed install task completes.

---

{#progress-1}

#### Progress

```cpp
ThreadSignal< void(int &)> Progress
```

Signals on overall progress update [0-100].

---

{#complete-3}

#### Complete

```cpp
ThreadSignal< void(LocalPackageVec &)> Complete
```

Signals on all tasks complete.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`InstallMonitor`](#installmonitor-1)  |  |
|  | [`InstallMonitor`](#installmonitor-2)  |  |
|  | [`InstallMonitor`](#installmonitor-3)  |  |
| `void` | [`addTask`](#addtask) `virtual` | Adds a task to monitor. |
| `void` | [`startAll`](#startall) `virtual` | Starts all monitored tasks. |
| `void` | [`cancelAll`](#cancelall) `virtual` | Cancels all monitored tasks. |
| `bool` | [`isComplete`](#iscomplete) `virtual` `const` | Returns true if all install tasks have completed, either successfully or unsuccessfully. |
| `InstallTaskPtrVec` | [`tasks`](#tasks) `virtual` `const` | Returns the list of monitored package tasks. |
| `LocalPackageVec` | [`packages`](#packages) `virtual` `const` | Returns the list of monitored packages. |

---

{#installmonitor-1}

#### InstallMonitor

```cpp
InstallMonitor()
```

---

{#installmonitor-2}

#### InstallMonitor

```cpp
InstallMonitor(const InstallMonitor &) = delete
```

---

{#installmonitor-3}

#### InstallMonitor

```cpp
InstallMonitor(InstallMonitor &&) = delete
```

---

{#addtask}

#### addTask

`virtual`

```cpp
virtual void addTask(InstallTask::Ptr task)
```

Adds a task to monitor.

---

{#startall}

#### startAll

`virtual`

```cpp
virtual void startAll()
```

Starts all monitored tasks.

---

{#cancelall}

#### cancelAll

`virtual`

```cpp
virtual void cancelAll()
```

Cancels all monitored tasks.

---

{#iscomplete}

#### isComplete

`virtual` `const`

```cpp
virtual bool isComplete() const
```

Returns true if all install tasks have completed, either successfully or unsuccessfully.

---

{#tasks}

#### tasks

`virtual` `const`

```cpp
virtual InstallTaskPtrVec tasks() const
```

Returns the list of monitored package tasks.

---

{#packages}

#### packages

`virtual` `const`

```cpp
virtual LocalPackageVec packages() const
```

Returns the list of monitored packages.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-10)  |  |
| `InstallTaskPtrVec` | [`_tasks`](#_tasks)  |  |
| `LocalPackageVec` | [`_packages`](#_packages)  |  |
| `int` | [`_progress`](#_progress)  |  |

---

{#_mutex-10}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_tasks}

#### _tasks

```cpp
InstallTaskPtrVec _tasks
```

---

{#_packages}

#### _packages

```cpp
LocalPackageVec _packages
```

---

{#_progress}

#### _progress

```cpp
int _progress
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onInstallStateChange`](#oninstallstatechange) `virtual` |  |
| `void` | [`onInstallComplete`](#oninstallcomplete) `virtual` |  |
| `void` | [`setProgress`](#setprogress) `virtual` |  |

---

{#oninstallstatechange}

#### onInstallStateChange

`virtual`

```cpp
virtual void onInstallStateChange(void * sender, InstallationState & state, const InstallationState & oldState)
```

---

{#oninstallcomplete}

#### onInstallComplete

`virtual`

```cpp
virtual void onInstallComplete(InstallTask & task)
```

---

{#setprogress}

#### setProgress

`virtual`

```cpp
virtual void setProgress(int value)
```

{#installtask}

## InstallTask

```cpp
#include <icy/pacm/installtask.h>
```

> **Inherits:** [`Runnable`](base.md#runnable), [`Stateful< InstallationState >`](base.md#stateful)

Downloads, extracts, and finalizes a single package installation.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(InstallTask &, int &)>` | [`Progress`](#progress-2)  | Signals on progress update [0-100]. |
| `Signal< void(InstallTask &)>` | [`Complete`](#complete-4)  | Signals on task completion for both success and failure cases. |

---

{#progress-2}

#### Progress

```cpp
Signal< void(InstallTask &, int &)> Progress
```

Signals on progress update [0-100].

---

{#complete-4}

#### Complete

```cpp
Signal< void(InstallTask &)> Complete
```

Signals on task completion for both success and failure cases.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`InstallTask`](#installtask-1)  | #### Parameters |
|  | [`InstallTask`](#installtask-2)  |  |
|  | [`InstallTask`](#installtask-3)  |  |
| `void` | [`start`](#start-10) `virtual` | Validates options, resolves the install directory, and launches the background runner.  |
| `void` | [`cancel`](#cancel-3) `virtual` | Transitions the task to the Cancelled state. |
| `void` | [`doDownload`](#dodownload) `virtual` | Downloads the package archive from the server. |
| `void` | [`doExtract`](#doextract) `virtual` | Extracts the downloaded package files to the intermediate directory. |
| `void` | [`doFinalize`](#dofinalize) `virtual` | Moves extracted files from the intermediate directory to the installation directory. |
| `void` | [`setComplete`](#setcomplete) `virtual` | Called when the task completes either successfully or in error. This will trigger destruction. |
| `Package::Asset` | [`getRemoteAsset`](#getremoteasset) `virtual` `const` | Returns the remote asset selected by the current [InstallOptions](#installoptions). Respects version and sdkVersion overrides; falls back to latestAsset(). |
| `LocalPackage *` | [`local`](#local) `virtual` `const` | Returns a pointer to the local package record. |
| `RemotePackage *` | [`remote`](#remote) `virtual` `const` | Returns a pointer to the remote package record. |
| `InstallOptions &` | [`options`](#options-4) `virtual` | Returns a reference to the installation options for this task. |
| `uv::Loop *` | [`loop`](#loop-7) `virtual` `const` | Returns the libuv event loop used for async operations. |
| `bool` | [`valid`](#valid-2) `virtual` `const` | Returns true if the task is not in a Failed state and both local and remote (if set) packages are valid. |
| `bool` | [`cancelled`](#cancelled-2) `virtual` `const` | Returns true if the task is in the Cancelled state. |
| `bool` | [`failed`](#failed) `virtual` `const` | Returns true if the task is in the Failed state. |
| `bool` | [`success`](#success-1) `virtual` `const` | Returns true if the task is in the Installed (success) state. |
| `bool` | [`complete`](#complete-5) `virtual` `const` | Returns true if the task has reached a terminal state (Installed, Cancelled, or Failed). |
| `int` | [`progress`](#progress-3) `virtual` `const` | Returns the current progress value in the range [0, 100]. |

---

{#installtask-1}

#### InstallTask

```cpp
InstallTask(PackageManager & manager, LocalPackage * local, RemotePackage * remote, const InstallOptions & options, uv::Loop * loop)
```

#### Parameters
* `manager` Owning [PackageManager](#packagemanager) instance. 

* `local` Local package record (must not be null). 

* `remote` Remote package record to install from (may be null for local-only ops). 

* `options` Version and path overrides for this installation. 

* `loop` libuv event loop to use for async HTTP downloads. 

#### Exceptions
* `std::runtime_error` if the task configuration is invalid.

---

{#installtask-2}

#### InstallTask

```cpp
InstallTask(const InstallTask &) = delete
```

---

{#installtask-3}

#### InstallTask

```cpp
InstallTask(InstallTask &&) = delete
```

---

{#start-10}

#### start

`virtual`

```cpp
virtual void start()
```

Validates options, resolves the install directory, and launches the background runner. 
#### Exceptions
* `std::runtime_error` if the requested version or SDK version asset is unavailable.

---

{#cancel-3}

#### cancel

`virtual`

```cpp
virtual void cancel()
```

Transitions the task to the Cancelled state.

---

{#dodownload}

#### doDownload

`virtual`

```cpp
virtual void doDownload()
```

Downloads the package archive from the server.

---

{#doextract}

#### doExtract

`virtual`

```cpp
virtual void doExtract()
```

Extracts the downloaded package files to the intermediate directory.

---

{#dofinalize}

#### doFinalize

`virtual`

```cpp
virtual void doFinalize()
```

Moves extracted files from the intermediate directory to the installation directory.

---

{#setcomplete}

#### setComplete

`virtual`

```cpp
virtual void setComplete()
```

Called when the task completes either successfully or in error. This will trigger destruction.

---

{#getremoteasset}

#### getRemoteAsset

`virtual` `const`

```cpp
virtual Package::Asset getRemoteAsset() const
```

Returns the remote asset selected by the current [InstallOptions](#installoptions). Respects version and sdkVersion overrides; falls back to latestAsset().

---

{#local}

#### local

`virtual` `const`

```cpp
virtual LocalPackage * local() const
```

Returns a pointer to the local package record.

---

{#remote}

#### remote

`virtual` `const`

```cpp
virtual RemotePackage * remote() const
```

Returns a pointer to the remote package record.

---

{#options-4}

#### options

`virtual`

```cpp
virtual InstallOptions & options()
```

Returns a reference to the installation options for this task.

---

{#loop-7}

#### loop

`virtual` `const`

```cpp
virtual uv::Loop * loop() const
```

Returns the libuv event loop used for async operations.

---

{#valid-2}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if the task is not in a Failed state and both local and remote (if set) packages are valid.

---

{#cancelled-2}

#### cancelled

`virtual` `const`

```cpp
virtual bool cancelled() const
```

Returns true if the task is in the Cancelled state.

---

{#failed}

#### failed

`virtual` `const`

```cpp
virtual bool failed() const
```

Returns true if the task is in the Failed state.

---

{#success-1}

#### success

`virtual` `const`

```cpp
virtual bool success() const
```

Returns true if the task is in the Installed (success) state.

---

{#complete-5}

#### complete

`virtual` `const`

```cpp
virtual bool complete() const
```

Returns true if the task has reached a terminal state (Installed, Cancelled, or Failed).

---

{#progress-3}

#### progress

`virtual` `const`

```cpp
virtual int progress() const
```

Returns the current progress value in the range [0, 100].

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-11)  |  |
| `Idler` | [`_runner`](#_runner-1)  |  |
| `icy::Error` | [`_error`](#_error-4)  |  |
| `PackageManager &` | [`_manager`](#_manager-3)  |  |
| `LocalPackage *` | [`_local`](#_local)  |  |
| `RemotePackage *` | [`_remote`](#_remote)  |  |
| `InstallOptions` | [`_options`](#_options-1)  |  |
| `int` | [`_progress`](#_progress-1)  |  |
| `bool` | [`_downloading`](#_downloading)  |  |
| `http::ClientConnection::Ptr` | [`_dlconn`](#_dlconn)  |  |
| `uv::Loop *` | [`_loop`](#_loop-2)  |  |

---

{#_mutex-11}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_runner-1}

#### _runner

```cpp
Idler _runner
```

---

{#_error-4}

#### _error

```cpp
icy::Error _error
```

---

{#_manager-3}

#### _manager

```cpp
PackageManager & _manager
```

---

{#_local}

#### _local

```cpp
LocalPackage * _local
```

---

{#_remote}

#### _remote

```cpp
RemotePackage * _remote
```

---

{#_options-1}

#### _options

```cpp
InstallOptions _options
```

---

{#_progress-1}

#### _progress

```cpp
int _progress
```

---

{#_downloading}

#### _downloading

```cpp
bool _downloading
```

---

{#_dlconn}

#### _dlconn

```cpp
http::ClientConnection::Ptr _dlconn
```

---

{#_loop-2}

#### _loop

```cpp
uv::Loop * _loop
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`run`](#run-5) `virtual` | Called asynchronously by the thread to do the work. |
| `void` | [`onStateChange`](#onstatechange) `virtual` |  |
| `void` | [`onDownloadProgress`](#ondownloadprogress) `virtual` |  |
| `void` | [`onDownloadComplete`](#ondownloadcomplete) `virtual` |  |
| `void` | [`setProgress`](#setprogress-1) `virtual` |  |

---

{#run-5}

#### run

`virtual`

```cpp
virtual void run()
```

Called asynchronously by the thread to do the work.

---

{#onstatechange}

#### onStateChange

`virtual`

```cpp
virtual void onStateChange(InstallationState & state, const InstallationState & oldState)
```

---

{#ondownloadprogress}

#### onDownloadProgress

`virtual`

```cpp
virtual void onDownloadProgress(const double & progress)
```

---

{#ondownloadcomplete}

#### onDownloadComplete

`virtual`

```cpp
virtual void onDownloadComplete(const http::Response & response)
```

---

{#setprogress-1}

#### setProgress

`virtual`

```cpp
virtual void setProgress(int value)
```

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr-14)  |  |

---

{#ptr-14}

#### Ptr

```cpp
std::shared_ptr< InstallTask > Ptr()
```

{#packagemanager}

## PackageManager

```cpp
#include <icy/pacm/packagemanager.h>
```

Loads package manifests and coordinates install, update, and uninstall workflows.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `Signal< void(const http::Response &)>` | [`RemotePackageResponse`](#remotepackageresponse)  | Events. |
| `Signal< void(LocalPackage &)>` | [`PackageUninstalled`](#packageuninstalled)  | Signals when a package is uninstalled. |
| `Signal< void(InstallTask &)>` | [`InstallTaskCreated`](#installtaskcreated)  | Signals when an installation task is created, before it is started. |
| `Signal< void(const InstallTask &)>` | [`InstallTaskComplete`](#installtaskcomplete)  | Signals when a package installation tasks completes, either successfully or in error. |

---

{#remotepackageresponse}

#### RemotePackageResponse

```cpp
Signal< void(const http::Response &)> RemotePackageResponse
```

Events.

Signals when the remote package list have been downloaded from the server.

---

{#packageuninstalled}

#### PackageUninstalled

```cpp
Signal< void(LocalPackage &)> PackageUninstalled
```

Signals when a package is uninstalled.

---

{#installtaskcreated}

#### InstallTaskCreated

```cpp
Signal< void(InstallTask &)> InstallTaskCreated
```

Signals when an installation task is created, before it is started.

---

{#installtaskcomplete}

#### InstallTaskComplete

```cpp
Signal< void(const InstallTask &)> InstallTaskComplete
```

Signals when a package installation tasks completes, either successfully or in error.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`PackageManager`](#packagemanager-1)  | #### Parameters |
|  | [`PackageManager`](#packagemanager-2)  |  |
|  | [`PackageManager`](#packagemanager-3)  |  |
| `void` | [`initialize`](#initialize) `virtual` | Initialization Methods. |
| `void` | [`uninitialize`](#uninitialize) `virtual` | Releases resources and cancels any in-progress tasks. |
| `bool` | [`initialized`](#initialized-3) `virtual` `const` | Returns true if [initialize()](#initialize) has been called successfully. |
| `void` | [`createDirectories`](#createdirectories) `virtual` | Creates the package manager directory structure if it does not already exist. |
| `void` | [`queryRemotePackages`](#queryremotepackages) `virtual` | Queries the server for a list of available packages. |
| `void` | [`loadLocalPackages`](#loadlocalpackages) `virtual` | Loads all local package manifests from file system. Clears all in memory package manifests. |
| `void` | [`loadLocalPackages`](#loadlocalpackages-1) `virtual` | Loads all local package manifests residing the the given directory. This method may be called multiple times for different paths because it does not clear in memory package manifests. |
| `bool` | [`saveLocalPackages`](#savelocalpackages) `virtual` | Saves all local package manifests to the data directory.  |
| `bool` | [`saveLocalPackage`](#savelocalpackage) `virtual` | Saves the local package manifest to the file system. |
| `void` | [`parseRemotePackages`](#parseremotepackages) `virtual` | Parse the remote packages from the given JSON data string. |
| `InstallTask::Ptr` | [`installPackage`](#installpackage) `virtual` | [Package](#package) Installation Methods. |
| `bool` | [`installPackages`](#installpackages) `virtual` | Installs multiple packages. The same options will be passed to each task. If a [InstallMonitor](#installmonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#packagemanager) does not take ownership of the [InstallMonitor](#installmonitor). |
| `InstallTask::Ptr` | [`updatePackage`](#updatepackage) `virtual` | Updates a single package. Throws an exception if the package does not exist. The returned [InstallTask](#installtask) must be started. |
| `bool` | [`updatePackages`](#updatepackages) `virtual` | Updates multiple packages. Throws an exception if the package does not exist. If a [InstallMonitor](#installmonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#packagemanager) does not take ownership of the [InstallMonitor](#installmonitor). |
| `bool` | [`updateAllPackages`](#updateallpackages) `virtual` | Updates all installed packages. |
| `bool` | [`uninstallPackages`](#uninstallpackages) `virtual` | Uninstalls multiple packages. |
| `bool` | [`uninstallPackage`](#uninstallpackage) `virtual` | Uninstalls a single package. |
| `bool` | [`hasUnfinalizedPackages`](#hasunfinalizedpackages) `virtual` | Returns true if there are updates available that have not yet been finalized. Packages may be unfinalized if there were files in use at the time of installation. |
| `bool` | [`finalizeInstallations`](#finalizeinstallations) `virtual` | Finalizes active installations by moving all package files to their target destination. If files are to be overwritten they must not be in use or finalization will fail. |
| `InstallTask::Ptr` | [`getInstallTask`](#getinstalltask) `virtual` `const` | [Task](base.md#task) Helper Methods. |
| `InstallTaskPtrVec` | [`tasks`](#tasks-1) `virtual` `const` | Returns a list of all tasks. |
| `void` | [`cancelAllTasks`](#cancelalltasks) `virtual` | Aborts all package installation tasks. All tasks must be aborted before clearing local or remote manifests. |
| `PackagePairVec` | [`getPackagePairs`](#getpackagepairs) `virtual` `const` | [Package](#package) Helper Methods. |
| `PackagePairVec` | [`getUpdatablePackagePairs`](#getupdatablepackagepairs) `virtual` `const` | Returns a list of package pairs which may be updated. All pairs will have both local and remote package pointers, and the remote version will be newer than the local version. |
| `PackagePair` | [`getPackagePair`](#getpackagepair) `virtual` `const` | Returns a local and remote package pair. An exception will be thrown if either the local or remote packages aren't available or are invalid. |
| `PackagePair` | [`getOrCreatePackagePair`](#getorcreatepackagepair) `virtual` | Returns a local and remote package pair. If the local package doesn't exist it will be created from the remote package. If the remote package doesn't exist a NotFoundException will be thrown. |
| `InstallTask::Ptr` | [`createInstallTask`](#createinstalltask) `virtual` | Creates a package installation task for the given pair. |
| `std::string` | [`installedPackageVersion`](#installedpackageversion) `virtual` `const` | Returns the version number of an installed package. Exceptions will be thrown if the package does not exist, or is not fully installed. |
| `Package::Asset` | [`getLatestInstallableAsset`](#getlatestinstallableasset) `virtual` `const` | Returns the best asset to install, or throws a descriptive exception if no updates are available, or if the package is already up-to-date. This method takes version and SDK locks into consideration. |
| `bool` | [`hasAvailableUpdates`](#hasavailableupdates) `virtual` `const` | Returns true if there are updates available for this package, false otherwise. |
| `void` | [`clearCache`](#clearcache)  | File Helper Methods. |
| `bool` | [`clearPackageCache`](#clearpackagecache)  | Clears a package archive from the local cache. |
| `bool` | [`clearCacheFile`](#clearcachefile)  | Clears a file from the local cache. |
| `bool` | [`hasCachedFile`](#hascachedfile)  | Checks if a package archive exists in the local cache. |
| `bool` | [`isSupportedFileType`](#issupportedfiletype)  | Checks if the file type is a supported package archive. |
| `std::string` | [`getCacheFilePath`](#getcachefilepath)  | Returns the full path of the cached file if it exists, or an empty path if the file doesn't exist. |
| `std::string` | [`getPackageDataDir`](#getpackagedatadir)  | Returns the package data directory for the given package ID. |
| `Options &` | [`options`](#options-5) `virtual` | Accessors. |
| `RemotePackageStore &` | [`remotePackages`](#remotepackages) `virtual` | Returns a reference to the in-memory remote package store. |
| `LocalPackageStore &` | [`localPackages`](#localpackages) `virtual` | Returns a reference to the in-memory local package store. |

---

{#packagemanager-1}

#### PackageManager

```cpp
PackageManager(const Options & options)
```

#### Parameters
* `options` [Configuration](base.md#configuration) for directories, endpoints, and credentials.

---

{#packagemanager-2}

#### PackageManager

```cpp
PackageManager(const PackageManager &) = delete
```

---

{#packagemanager-3}

#### PackageManager

```cpp
PackageManager(PackageManager &&) = delete
```

---

{#initialize}

#### initialize

`virtual`

```cpp
virtual void initialize()
```

Initialization Methods.

Initializes the package manager: creates directories, loads local manifests, and queries the remote package index.

---

{#uninitialize}

#### uninitialize

`virtual`

```cpp
virtual void uninitialize()
```

Releases resources and cancels any in-progress tasks.

---

{#initialized-3}

#### initialized

`virtual` `const`

```cpp
virtual bool initialized() const
```

Returns true if [initialize()](#initialize) has been called successfully.

---

{#createdirectories}

#### createDirectories

`virtual`

```cpp
virtual void createDirectories()
```

Creates the package manager directory structure if it does not already exist.

---

{#queryremotepackages}

#### queryRemotePackages

`virtual`

```cpp
virtual void queryRemotePackages()
```

Queries the server for a list of available packages.

---

{#loadlocalpackages}

#### loadLocalPackages

`virtual`

```cpp
virtual void loadLocalPackages()
```

Loads all local package manifests from file system. Clears all in memory package manifests.

---

{#loadlocalpackages-1}

#### loadLocalPackages

`virtual`

```cpp
virtual void loadLocalPackages(const std::string & dir)
```

Loads all local package manifests residing the the given directory. This method may be called multiple times for different paths because it does not clear in memory package manifests.

---

{#savelocalpackages}

#### saveLocalPackages

`virtual`

```cpp
virtual bool saveLocalPackages(bool whiny)
```

Saves all local package manifests to the data directory. 
#### Parameters
* `whiny` If true, re-throws on write error; otherwise returns false. 

#### Returns
true on success.

---

{#savelocalpackage}

#### saveLocalPackage

`virtual`

```cpp
virtual bool saveLocalPackage(LocalPackage & package, bool whiny)
```

Saves the local package manifest to the file system.

---

{#parseremotepackages}

#### parseRemotePackages

`virtual`

```cpp
virtual void parseRemotePackages(const std::string & data)
```

Parse the remote packages from the given JSON data string.

---

{#installpackage}

#### installPackage

`virtual`

```cpp
virtual InstallTask::Ptr installPackage(const std::string & name, const InstallOptions & options)
```

[Package](#package) Installation Methods.

Installs a single package. The returned [InstallTask](#installtask) must be started. If the package is already up-to-date, a nullptr will be returned. Any other error will throw a std::runtime_error.

---

{#installpackages}

#### installPackages

`virtual`

```cpp
virtual bool installPackages(const StringVec & ids, const InstallOptions & options, InstallMonitor * monitor, bool whiny)
```

Installs multiple packages. The same options will be passed to each task. If a [InstallMonitor](#installmonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#packagemanager) does not take ownership of the [InstallMonitor](#installmonitor).

---

{#updatepackage}

#### updatePackage

`virtual`

```cpp
virtual InstallTask::Ptr updatePackage(const std::string & name, const InstallOptions & options)
```

Updates a single package. Throws an exception if the package does not exist. The returned [InstallTask](#installtask) must be started.

---

{#updatepackages}

#### updatePackages

`virtual`

```cpp
virtual bool updatePackages(const StringVec & ids, const InstallOptions & options, InstallMonitor * monitor, bool whiny)
```

Updates multiple packages. Throws an exception if the package does not exist. If a [InstallMonitor](#installmonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#packagemanager) does not take ownership of the [InstallMonitor](#installmonitor).

---

{#updateallpackages}

#### updateAllPackages

`virtual`

```cpp
virtual bool updateAllPackages(bool whiny)
```

Updates all installed packages.

---

{#uninstallpackages}

#### uninstallPackages

`virtual`

```cpp
virtual bool uninstallPackages(const StringVec & ids, bool whiny)
```

Uninstalls multiple packages.

---

{#uninstallpackage}

#### uninstallPackage

`virtual`

```cpp
virtual bool uninstallPackage(const std::string & id, bool whiny)
```

Uninstalls a single package.

---

{#hasunfinalizedpackages}

#### hasUnfinalizedPackages

`virtual`

```cpp
virtual bool hasUnfinalizedPackages()
```

Returns true if there are updates available that have not yet been finalized. Packages may be unfinalized if there were files in use at the time of installation.

---

{#finalizeinstallations}

#### finalizeInstallations

`virtual`

```cpp
virtual bool finalizeInstallations(bool whiny)
```

Finalizes active installations by moving all package files to their target destination. If files are to be overwritten they must not be in use or finalization will fail.

---

{#getinstalltask}

#### getInstallTask

`virtual` `const`

```cpp
virtual InstallTask::Ptr getInstallTask(const std::string & id) const
```

[Task](base.md#task) Helper Methods.

Gets the install task for the given package ID.

---

{#tasks-1}

#### tasks

`virtual` `const`

```cpp
virtual InstallTaskPtrVec tasks() const
```

Returns a list of all tasks.

---

{#cancelalltasks}

#### cancelAllTasks

`virtual`

```cpp
virtual void cancelAllTasks()
```

Aborts all package installation tasks. All tasks must be aborted before clearing local or remote manifests.

---

{#getpackagepairs}

#### getPackagePairs

`virtual` `const`

```cpp
virtual PackagePairVec getPackagePairs() const
```

[Package](#package) Helper Methods.

Returns all package pairs, valid or invalid. Some pairs may not have both local and remote package pointers.

---

{#getupdatablepackagepairs}

#### getUpdatablePackagePairs

`virtual` `const`

```cpp
virtual PackagePairVec getUpdatablePackagePairs() const
```

Returns a list of package pairs which may be updated. All pairs will have both local and remote package pointers, and the remote version will be newer than the local version.

---

{#getpackagepair}

#### getPackagePair

`virtual` `const`

```cpp
virtual PackagePair getPackagePair(const std::string & id, bool whiny) const
```

Returns a local and remote package pair. An exception will be thrown if either the local or remote packages aren't available or are invalid.

---

{#getorcreatepackagepair}

#### getOrCreatePackagePair

`virtual`

```cpp
virtual PackagePair getOrCreatePackagePair(const std::string & id)
```

Returns a local and remote package pair. If the local package doesn't exist it will be created from the remote package. If the remote package doesn't exist a NotFoundException will be thrown.

---

{#createinstalltask}

#### createInstallTask

`virtual`

```cpp
virtual InstallTask::Ptr createInstallTask(PackagePair & pair, const InstallOptions & options)
```

Creates a package installation task for the given pair.

---

{#installedpackageversion}

#### installedPackageVersion

`virtual` `const`

```cpp
virtual std::string installedPackageVersion(const std::string & id) const
```

Returns the version number of an installed package. Exceptions will be thrown if the package does not exist, or is not fully installed.

---

{#getlatestinstallableasset}

#### getLatestInstallableAsset

`virtual` `const`

```cpp
virtual Package::Asset getLatestInstallableAsset(const PackagePair & pair, const InstallOptions & options) const
```

Returns the best asset to install, or throws a descriptive exception if no updates are available, or if the package is already up-to-date. This method takes version and SDK locks into consideration.

---

{#hasavailableupdates}

#### hasAvailableUpdates

`virtual` `const`

```cpp
virtual bool hasAvailableUpdates(const PackagePair & pair) const
```

Returns true if there are updates available for this package, false otherwise.

---

{#clearcache}

#### clearCache

```cpp
void clearCache()
```

File Helper Methods.

Clears all files in the cache directory.

---

{#clearpackagecache}

#### clearPackageCache

```cpp
bool clearPackageCache(LocalPackage & package)
```

Clears a package archive from the local cache.

---

{#clearcachefile}

#### clearCacheFile

```cpp
bool clearCacheFile(std::string_view fileName, bool whiny)
```

Clears a file from the local cache.

---

{#hascachedfile}

#### hasCachedFile

```cpp
bool hasCachedFile(Package::Asset & asset)
```

Checks if a package archive exists in the local cache.

---

{#issupportedfiletype}

#### isSupportedFileType

```cpp
bool isSupportedFileType(std::string_view fileName)
```

Checks if the file type is a supported package archive.

---

{#getcachefilepath}

#### getCacheFilePath

```cpp
std::string getCacheFilePath(std::string_view fileName)
```

Returns the full path of the cached file if it exists, or an empty path if the file doesn't exist.

---

{#getpackagedatadir}

#### getPackageDataDir

```cpp
std::string getPackageDataDir(std::string_view id)
```

Returns the package data directory for the given package ID.

---

{#options-5}

#### options

`virtual`

```cpp
virtual Options & options()
```

Accessors.

Returns a reference to the current options.

---

{#remotepackages}

#### remotePackages

`virtual`

```cpp
virtual RemotePackageStore & remotePackages()
```

Returns a reference to the in-memory remote package store.

---

{#localpackages}

#### localPackages

`virtual`

```cpp
virtual LocalPackageStore & localPackages()
```

Returns a reference to the in-memory local package store.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::mutex` | [`_mutex`](#_mutex-12)  |  |
| `LocalPackageStore` | [`_localPackages`](#_localpackages)  |  |
| `RemotePackageStore` | [`_remotePackages`](#_remotepackages)  |  |
| `InstallTaskPtrVec` | [`_tasks`](#_tasks-1)  |  |
| `Options` | [`_options`](#_options-2)  |  |

---

{#_mutex-12}

#### _mutex

```cpp
std::mutex _mutex
```

---

{#_localpackages}

#### _localPackages

```cpp
LocalPackageStore _localPackages
```

---

{#_remotepackages}

#### _remotePackages

```cpp
RemotePackageStore _remotePackages
```

---

{#_tasks-1}

#### _tasks

```cpp
InstallTaskPtrVec _tasks
```

---

{#_options-2}

#### _options

```cpp
Options _options
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onPackageInstallComplete`](#onpackageinstallcomplete)  | Callbacks. |

---

{#onpackageinstallcomplete}

#### onPackageInstallComplete

```cpp
void onPackageInstallComplete(InstallTask & task)
```

Callbacks.

{#options-6}

## Options

```cpp
#include <icy/pacm/packagemanager.h>
```

Startup configuration for repository endpoints, credentials, and directories.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`endpoint`](#endpoint)  | The HTTP server endpoint. |
| `std::string` | [`indexURI`](#indexuri)  | The HTTP server URI for querying packages JSON. |
| `std::string` | [`httpUsername`](#httpusername)  | Username for HTTP basic auth. |
| `std::string` | [`httpPassword`](#httppassword)  | PAssword for HTTP basic auth. |
| `std::string` | [`httpOAuthToken`](#httpoauthtoken)  | Will be used instead of HTTP basic if provided. |
| `std::string` | [`tempDir`](#tempdir)  | Directory where package files will be downloaded and extracted. |
| `std::string` | [`dataDir`](#datadir)  | Directory where package manifests will be kept. |
| `std::string` | [`installDir`](#installdir-2)  | Directory where packages will be installed. |
| `std::string` | [`platform`](#platform)  | Platform (win32, linux, mac) |
| `std::string` | [`checksumAlgorithm`](#checksumalgorithm)  | Checksum algorithm (MDS/SHA1) |
| `bool` | [`clearFailedCache`](#clearfailedcache)  | This flag tells the package manager weather or not to clear the package cache if installation fails. |

---

{#endpoint}

#### endpoint

```cpp
std::string endpoint
```

The HTTP server endpoint.

---

{#indexuri}

#### indexURI

```cpp
std::string indexURI
```

The HTTP server URI for querying packages JSON.

---

{#httpusername}

#### httpUsername

```cpp
std::string httpUsername
```

Username for HTTP basic auth.

---

{#httppassword}

#### httpPassword

```cpp
std::string httpPassword
```

PAssword for HTTP basic auth.

---

{#httpoauthtoken}

#### httpOAuthToken

```cpp
std::string httpOAuthToken
```

Will be used instead of HTTP basic if provided.

---

{#tempdir}

#### tempDir

```cpp
std::string tempDir
```

Directory where package files will be downloaded and extracted.

---

{#datadir}

#### dataDir

```cpp
std::string dataDir
```

Directory where package manifests will be kept.

---

{#installdir-2}

#### installDir

```cpp
std::string installDir
```

Directory where packages will be installed.

---

{#platform}

#### platform

```cpp
std::string platform
```

Platform (win32, linux, mac)

---

{#checksumalgorithm}

#### checksumAlgorithm

```cpp
std::string checksumAlgorithm
```

Checksum algorithm (MDS/SHA1)

---

{#clearfailedcache}

#### clearFailedCache

```cpp
bool clearFailedCache
```

This flag tells the package manager weather or not to clear the package cache if installation fails.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Options`](#options-7) `inline` |  |

---

{#options-7}

#### Options

`inline`

```cpp
inline Options(const std::string & root)
```

{#installationstate}

## InstallationState

```cpp
#include <icy/pacm/installtask.h>
```

> **Inherits:** [`State`](base.md#state)

[State](base.md#state) machine states for package installation.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`str`](#str-2) `const` `inline` | Converts a state ID to its string representation.  |

---

{#str-2}

#### str

`const` `inline`

```cpp
inline std::string str(unsigned int id) const
```

Converts a state ID to its string representation. 
#### Parameters
* `id` One of the Type enum values. 

#### Returns
Human-readable state name, or "undefined" for unknown values.

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-10)  |  |

---

{#type-10}

#### Type

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `None` |  |
| `Downloading` |  |
| `Extracting` |  |
| `Finalizing` |  |
| `Installed` |  |
| `Cancelled` |  |
| `Failed` |  |

{#installoptions}

## InstallOptions

```cpp
#include <icy/pacm/installtask.h>
```

[Package](#package) installation options.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`version`](#version-3)  | If set then the given package version will be installed. |
| `std::string` | [`sdkVersion`](#sdkversion)  | If set then the latest package version for given SDK version will be installed. |
| `std::string` | [`installDir`](#installdir)  | Install to the given location, otherwise the manager default `installDir` will be used. |

---

{#version-3}

#### version

```cpp
std::string version
```

If set then the given package version will be installed.

---

{#sdkversion}

#### sdkVersion

```cpp
std::string sdkVersion
```

If set then the latest package version for given SDK version will be installed.

---

{#installdir}

#### installDir

```cpp
std::string installDir
```

Install to the given location, otherwise the manager default `installDir` will be used.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`InstallOptions`](#installoptions-1) `inline` |  |

---

{#installoptions-1}

#### InstallOptions

`inline`

```cpp
inline InstallOptions()
```

{#localpackage}

## LocalPackage

```cpp
#include <icy/pacm/package.h>
```

> **Inherits:** [`Package`](#package)

[Package](#package) metadata for an installed package on the local filesystem.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`LocalPackage`](#localpackage-1)  | Constructs an empty local package. |
|  | [`LocalPackage`](#localpackage-2)  | Constructs a local package from an existing JSON value.  |
|  | [`LocalPackage`](#localpackage-3)  | Create the local package from the remote package reference with the following manipulations. 1) Add a local manifest element. 2) Remove asset mirror elements. |
| `void` | [`setState`](#setstate-1) `virtual` | Set's the overall package state. Possible values are: Installing, Installed, Failed, Uninstalled. If the packages completes while still Installing, this means the package has yet to be finalized. |
| `void` | [`setInstallState`](#setinstallstate) `virtual` | Set's the package installation state. See [InstallationState](#installationstate) for possible values. |
| `void` | [`setInstallDir`](#setinstalldir) `virtual` | Set's the installation directory for this package. |
| `void` | [`setInstalledAsset`](#setinstalledasset) `virtual` | Sets the installed asset, once installed. This method also sets the version. |
| `void` | [`setVersion`](#setversion-2) `virtual` | Sets the current version of the local package. Installation must be complete. |
| `void` | [`setVersionLock`](#setversionlock) `virtual` | Locks the package at the given version. Once set this package will not be updated past the given version. Pass an empty string to remove the lock. |
| `void` | [`setSDKVersionLock`](#setsdkversionlock) `virtual` | Locks the package at the given SDK version. Once set this package will only update to the most recent version with given SDK version. Pass an empty string to remove the lock. |
| `std::string` | [`version`](#version-4) `virtual` `const` | Returns the installed package version. |
| `std::string` | [`state`](#state-2) `virtual` `const` | Returns the current state of this package. |
| `std::string` | [`installState`](#installstate) `virtual` `const` | Returns the installation state of this package. |
| `std::string` | [`installDir`](#installdir-1) `virtual` `const` | Returns the installation directory for this package. |
| `std::string` | [`versionLock`](#versionlock) `virtual` `const` | Returns the pinned version string, or empty if no lock is set. |
| `std::string` | [`sdkLockedVersion`](#sdklockedversion) `virtual` `const` | Returns the pinned SDK version string, or empty if no lock is set. |
| `Asset` | [`asset`](#asset) `virtual` | Returns the currently installed asset, if any. If none, the returned asset will be empty(). |
| `bool` | [`isInstalled`](#isinstalled) `virtual` `const` | Returns true or false depending on weather or not the package is installed successfully. False if package is in Failed state. |
| `bool` | [`isFailed`](#isfailed) `virtual` `const` | Returns true if the package state is "Failed". |
| `Manifest` | [`manifest`](#manifest) `virtual` | Returns the installation manifest. |
| `bool` | [`verifyInstallManifest`](#verifyinstallmanifest) `virtual` |  |
| `std::string` | [`getInstalledFilePath`](#getinstalledfilepath) `virtual` | Returns the full full path of the installed file. Thrown an exception if the install directory is unset. |
| `json::Value &` | [`errors`](#errors-2) `virtual` | Returns a reference to the JSON array of accumulated error messages. |
| `void` | [`addError`](#adderror) `virtual` | Appends `message` to the errors array.  |
| `std::string` | [`lastError`](#lasterror) `virtual` `const` | Returns the most recently added error message, or empty if none. |
| `void` | [`clearErrors`](#clearerrors) `virtual` | Clears all recorded error messages. |
| `bool` | [`valid`](#valid-3) `virtual` `const` | Returns true if id, name and type are all non-empty. |

---

{#localpackage-1}

#### LocalPackage

```cpp
LocalPackage()
```

Constructs an empty local package.

---

{#localpackage-2}

#### LocalPackage

```cpp
LocalPackage(const json::Value & src)
```

Constructs a local package from an existing JSON value. 
#### Parameters
* `src` JSON object containing local package fields.

---

{#localpackage-3}

#### LocalPackage

```cpp
LocalPackage(const RemotePackage & src)
```

Create the local package from the remote package reference with the following manipulations. 1) Add a local manifest element. 2) Remove asset mirror elements.

---

{#setstate-1}

#### setState

`virtual`

```cpp
virtual void setState(const std::string & state)
```

Set's the overall package state. Possible values are: Installing, Installed, Failed, Uninstalled. If the packages completes while still Installing, this means the package has yet to be finalized.

---

{#setinstallstate}

#### setInstallState

`virtual`

```cpp
virtual void setInstallState(const std::string & state)
```

Set's the package installation state. See [InstallationState](#installationstate) for possible values.

---

{#setinstalldir}

#### setInstallDir

`virtual`

```cpp
virtual void setInstallDir(const std::string & dir)
```

Set's the installation directory for this package.

---

{#setinstalledasset}

#### setInstalledAsset

`virtual`

```cpp
virtual void setInstalledAsset(const Package::Asset & installedRemoteAsset)
```

Sets the installed asset, once installed. This method also sets the version.

---

{#setversion-2}

#### setVersion

`virtual`

```cpp
virtual void setVersion(const std::string & version)
```

Sets the current version of the local package. Installation must be complete.

---

{#setversionlock}

#### setVersionLock

`virtual`

```cpp
virtual void setVersionLock(const std::string & version)
```

Locks the package at the given version. Once set this package will not be updated past the given version. Pass an empty string to remove the lock.

---

{#setsdkversionlock}

#### setSDKVersionLock

`virtual`

```cpp
virtual void setSDKVersionLock(const std::string & version)
```

Locks the package at the given SDK version. Once set this package will only update to the most recent version with given SDK version. Pass an empty string to remove the lock.

---

{#version-4}

#### version

`virtual` `const`

```cpp
virtual std::string version() const
```

Returns the installed package version.

---

{#state-2}

#### state

`virtual` `const`

```cpp
virtual std::string state() const
```

Returns the current state of this package.

---

{#installstate}

#### installState

`virtual` `const`

```cpp
virtual std::string installState() const
```

Returns the installation state of this package.

---

{#installdir-1}

#### installDir

`virtual` `const`

```cpp
virtual std::string installDir() const
```

Returns the installation directory for this package.

---

{#versionlock}

#### versionLock

`virtual` `const`

```cpp
virtual std::string versionLock() const
```

Returns the pinned version string, or empty if no lock is set.

---

{#sdklockedversion}

#### sdkLockedVersion

`virtual` `const`

```cpp
virtual std::string sdkLockedVersion() const
```

Returns the pinned SDK version string, or empty if no lock is set.

---

{#asset}

#### asset

`virtual`

```cpp
virtual Asset asset()
```

Returns the currently installed asset, if any. If none, the returned asset will be empty().

---

{#isinstalled}

#### isInstalled

`virtual` `const`

```cpp
virtual bool isInstalled() const
```

Returns true or false depending on weather or not the package is installed successfully. False if package is in Failed state.

---

{#isfailed}

#### isFailed

`virtual` `const`

```cpp
virtual bool isFailed() const
```

Returns true if the package state is "Failed".

---

{#manifest}

#### manifest

`virtual`

```cpp
virtual Manifest manifest()
```

Returns the installation manifest.

---

{#verifyinstallmanifest}

#### verifyInstallManifest

`virtual`

```cpp
virtual bool verifyInstallManifest(bool allowEmpty)
```

---

{#getinstalledfilepath}

#### getInstalledFilePath

`virtual`

```cpp
virtual std::string getInstalledFilePath(const std::string & fileName, bool whiny)
```

Returns the full full path of the installed file. Thrown an exception if the install directory is unset.

---

{#errors-2}

#### errors

`virtual`

```cpp
virtual json::Value & errors()
```

Returns a reference to the JSON array of accumulated error messages.

---

{#adderror}

#### addError

`virtual`

```cpp
virtual void addError(const std::string & message)
```

Appends `message` to the errors array. 
#### Parameters
* `message` [Error](base.md#error) description to record.

---

{#lasterror}

#### lastError

`virtual` `const`

```cpp
virtual std::string lastError() const
```

Returns the most recently added error message, or empty if none.

---

{#clearerrors}

#### clearErrors

`virtual`

```cpp
virtual void clearErrors()
```

Clears all recorded error messages.

---

{#valid-3}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if id, name and type are all non-empty.

{#manifest-1}

## Manifest

```cpp
#include <icy/pacm/package.h>
```

[Manifest](#manifest-1) of installed files recorded for a local package.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `json::Value &` | [`root`](#root-1)  |  |

---

{#root-1}

#### root

```cpp
json::Value & root
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Manifest`](#manifest-2)  | #### Parameters |
| `bool` | [`empty`](#empty-1) `virtual` `const` | Returns true if the manifest contains no file entries. |
| `void` | [`addFile`](#addfile) `virtual` | Appends `path` to the manifest file list.  |

---

{#manifest-2}

#### Manifest

```cpp
Manifest(json::Value & src)
```

#### Parameters
* `src` JSON array node that backs this manifest.

---

{#empty-1}

#### empty

`virtual` `const`

```cpp
virtual bool empty() const
```

Returns true if the manifest contains no file entries.

---

{#addfile}

#### addFile

`virtual`

```cpp
virtual void addFile(const std::string & path)
```

Appends `path` to the manifest file list. 
#### Parameters
* `path` Relative path of an installed file.

{#package}

## Package

```cpp
#include <icy/pacm/package.h>
```

> **Inherits:** `Value`
> **Subclassed by:** [`LocalPackage`](#localpackage), [`RemotePackage`](#remotepackage)

JSON-backed package metadata shared by local and remote package records.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Package`](#package-1)  | Constructs an empty package. |
|  | [`Package`](#package-2)  | Constructs a package from an existing JSON value.  |
| `std::string` | [`id`](#id-2) `virtual` `const` | Returns the package unique identifier. |
| `std::string` | [`name`](#name-6) `virtual` `const` | Returns the package display name. |
| `std::string` | [`type`](#type-11) `virtual` `const` | Returns the package type (e.g. "plugin", "asset"). |
| `std::string` | [`author`](#author) `virtual` `const` | Returns the package author string. |
| `std::string` | [`description`](#description) `virtual` `const` | Returns the package description string. |
| `bool` | [`valid`](#valid-4) `virtual` `const` | Returns true if id, name and type are all non-empty. |
| `json::Value` | [`toJson`](#tojson) `virtual` `const` | Returns a plain JSON copy of this package object. |
| `void` | [`print`](#print-9) `virtual` `const` | Dumps the JSON representation of this package to `ost`.  |

---

{#package-1}

#### Package

```cpp
Package()
```

Constructs an empty package.

---

{#package-2}

#### Package

```cpp
Package(const json::Value & src)
```

Constructs a package from an existing JSON value. 
#### Parameters
* `src` JSON object containing package fields.

---

{#id-2}

#### id

`virtual` `const`

```cpp
virtual std::string id() const
```

Returns the package unique identifier.

---

{#name-6}

#### name

`virtual` `const`

```cpp
virtual std::string name() const
```

Returns the package display name.

---

{#type-11}

#### type

`virtual` `const`

```cpp
virtual std::string type() const
```

Returns the package type (e.g. "plugin", "asset").

---

{#author}

#### author

`virtual` `const`

```cpp
virtual std::string author() const
```

Returns the package author string.

---

{#description}

#### description

`virtual` `const`

```cpp
virtual std::string description() const
```

Returns the package description string.

---

{#valid-4}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if id, name and type are all non-empty.

---

{#tojson}

#### toJson

`virtual` `const`

```cpp
virtual json::Value toJson() const
```

Returns a plain JSON copy of this package object.

---

{#print-9}

#### print

`virtual` `const`

```cpp
virtual void print(std::ostream & ost) const
```

Dumps the JSON representation of this package to `ost`. 
#### Parameters
* `ost` Output stream.

{#asset-1}

## Asset

```cpp
#include <icy/pacm/package.h>
```

Archive asset metadata for a specific package build.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `json::Value &` | [`root`](#root-2)  |  |

---

{#root-2}

#### root

```cpp
json::Value & root
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Asset`](#asset-2)  | #### Parameters |
|  | [`Asset`](#asset-3)  |  |
| `std::string` | [`fileName`](#filename-2) `virtual` `const` | Returns the archive file name (e.g. "my-plugin-1.0.0.zip"). |
| `std::string` | [`version`](#version-5) `virtual` `const` | Returns the package version string (e.g. "1.0.0"). |
| `std::string` | [`sdkVersion`](#sdkversion-1) `virtual` `const` | Returns the SDK version this asset was built against (e.g. "2.0.0"). |
| `std::string` | [`checksum`](#checksum) `virtual` `const` | Returns the asset checksum string, or empty if none is set. |
| `std::string` | [`url`](#url-8) `virtual` `const` | Returns the download URL from the mirror list at `index`.  |
| `int` | [`fileSize`](#filesize-1) `virtual` `const` | Returns the uncompressed file size in bytes, or 0 if not set. |
| `bool` | [`valid`](#valid-5) `virtual` `const` | Returns true if the asset has the minimum required fields (file-name, version, mirrors). |
| `void` | [`print`](#print-10) `virtual` `const` | Writes the raw JSON of this asset to `ost`.  |
| `Asset &` | [`operator=`](#operator-18) `virtual` | Copies the backing JSON node from `r`.  |
| `bool` | [`operator==`](#operator-19) `virtual` `const` | Returns true if file name, version and checksum all match `r`. |

---

{#asset-2}

#### Asset

```cpp
Asset(json::Value & src)
```

#### Parameters
* `src` JSON object node that backs this asset.

---

{#asset-3}

#### Asset

```cpp
Asset(const Asset &) = default
```

---

{#filename-2}

#### fileName

`virtual` `const`

```cpp
virtual std::string fileName() const
```

Returns the archive file name (e.g. "my-plugin-1.0.0.zip").

---

{#version-5}

#### version

`virtual` `const`

```cpp
virtual std::string version() const
```

Returns the package version string (e.g. "1.0.0").

---

{#sdkversion-1}

#### sdkVersion

`virtual` `const`

```cpp
virtual std::string sdkVersion() const
```

Returns the SDK version this asset was built against (e.g. "2.0.0").

---

{#checksum}

#### checksum

`virtual` `const`

```cpp
virtual std::string checksum() const
```

Returns the asset checksum string, or empty if none is set.

---

{#url-8}

#### url

`virtual` `const`

```cpp
virtual std::string url(int index) const
```

Returns the download URL from the mirror list at `index`. 
#### Parameters
* `index` Zero-based index into the mirrors array.

---

{#filesize-1}

#### fileSize

`virtual` `const`

```cpp
virtual int fileSize() const
```

Returns the uncompressed file size in bytes, or 0 if not set.

---

{#valid-5}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if the asset has the minimum required fields (file-name, version, mirrors).

---

{#print-10}

#### print

`virtual` `const`

```cpp
virtual void print(std::ostream & ost) const
```

Writes the raw JSON of this asset to `ost`. 
#### Parameters
* `ost` Output stream.

---

{#operator-18}

#### operator=

`virtual`

```cpp
virtual Asset & operator=(const Asset & r)
```

Copies the backing JSON node from `r`. 
#### Parameters
* `r` Source asset to copy from.

---

{#operator-19}

#### operator==

`virtual` `const`

```cpp
virtual bool operator==(const Asset & r) const
```

Returns true if file name, version and checksum all match `r`.

{#packagepair}

## PackagePair

```cpp
#include <icy/pacm/package.h>
```

Pairing of the installed and remote metadata for the same package ID.

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `LocalPackage *` | [`local`](#local-1)  | Returns true if there are no possible updates for this package, false otherwise. |
| `RemotePackage *` | [`remote`](#remote-1)  |  |

---

{#local-1}

#### local

```cpp
LocalPackage * local
```

Returns true if there are no possible updates for this package, false otherwise.

---

{#remote-1}

#### remote

```cpp
RemotePackage * remote
```

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`PackagePair`](#packagepair-1)  | #### Parameters |
| `bool` | [`valid`](#valid-6) `virtual` `const` | Returns true if at least one of local/remote is set and that pointer is itself [valid()](#valid-6). |
| `std::string` | [`id`](#id-3) `const` | Returns the package ID, preferring the local package if available. |
| `std::string` | [`name`](#name-7) `const` | Returns the package display name, preferring the local package if available. |
| `std::string` | [`type`](#type-12) `const` | Returns the package type, preferring the local package if available. |
| `std::string` | [`author`](#author-1) `const` | Returns the package author, preferring the local package if available. |

---

{#packagepair-1}

#### PackagePair

```cpp
PackagePair(LocalPackage * local, RemotePackage * remote)
```

#### Parameters
* `local` Pointer to the locally installed package, or nullptr if not installed. 

* `remote` Pointer to the remote package record, or nullptr if not known.

---

{#valid-6}

#### valid

`virtual` `const`

```cpp
virtual bool valid() const
```

Returns true if at least one of local/remote is set and that pointer is itself [valid()](#valid-6).

---

{#id-3}

#### id

`const`

```cpp
std::string id() const
```

Returns the package ID, preferring the local package if available.

---

{#name-7}

#### name

`const`

```cpp
std::string name() const
```

Returns the package display name, preferring the local package if available.

---

{#type-12}

#### type

`const`

```cpp
std::string type() const
```

Returns the package type, preferring the local package if available.

---

{#author-1}

#### author

`const`

```cpp
std::string author() const
```

Returns the package author, preferring the local package if available.

{#remotepackage}

## RemotePackage

```cpp
#include <icy/pacm/package.h>
```

> **Inherits:** [`Package`](#package)

[Package](#package) metadata loaded from the remote package index.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`RemotePackage`](#remotepackage-1)  | Constructs an empty remote package. |
|  | [`RemotePackage`](#remotepackage-2)  | Constructs a remote package from an existing JSON value.  |
| `json::Value &` | [`assets`](#assets) `virtual` | Returns a reference to the "assets" JSON array node. |
| `Asset` | [`latestAsset`](#latestasset) `virtual` | Returns the latest asset for this package. For local packages this is the currently installed version. For remote packages this is the latest available version. Throws an exception if no asset exists. |
| `Asset` | [`assetVersion`](#assetversion) `virtual` | Returns the latest asset for the given package version. Throws an exception if no asset exists. |
| `Asset` | [`latestSDKAsset`](#latestsdkasset) `virtual` | Returns the latest asset for the given SDK version. This method is for safely installing plug-ins which must be compiled against a specific SDK version. The package JSON must have a "sdk-version" member for this function to work as intended. Throws an exception if no asset exists. |

---

{#remotepackage-1}

#### RemotePackage

```cpp
RemotePackage()
```

Constructs an empty remote package.

---

{#remotepackage-2}

#### RemotePackage

```cpp
RemotePackage(const json::Value & src)
```

Constructs a remote package from an existing JSON value. 
#### Parameters
* `src` JSON object containing remote package fields.

---

{#assets}

#### assets

`virtual`

```cpp
virtual json::Value & assets()
```

Returns a reference to the "assets" JSON array node.

---

{#latestasset}

#### latestAsset

`virtual`

```cpp
virtual Asset latestAsset()
```

Returns the latest asset for this package. For local packages this is the currently installed version. For remote packages this is the latest available version. Throws an exception if no asset exists.

---

{#assetversion}

#### assetVersion

`virtual`

```cpp
virtual Asset assetVersion(const std::string & version)
```

Returns the latest asset for the given package version. Throws an exception if no asset exists.

---

{#latestsdkasset}

#### latestSDKAsset

`virtual`

```cpp
virtual Asset latestSDKAsset(const std::string & version)
```

Returns the latest asset for the given SDK version. This method is for safely installing plug-ins which must be compiled against a specific SDK version. The package JSON must have a "sdk-version" member for this function to work as intended. Throws an exception if no asset exists.

