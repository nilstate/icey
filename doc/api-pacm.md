# pacm 

### Classes

| Name | Description |
|------|-------------|
| [`InstallMonitor`](#classscy_1_1pacm_1_1InstallMonitor) |  |
| [`InstallTask`](#classscy_1_1pacm_1_1InstallTask) | This class implements the package installation procedure. |
| [`PackageManager`](#classscy_1_1pacm_1_1PackageManager) | The [Package](#structscy_1_1pacm_1_1Package) Manager provides an interface for managing, installing, updating and uninstalling Pacm packages. |
| [`InstallationState`](#structscy_1_1pacm_1_1InstallationState) |  |
| [`InstallOptions`](#structscy_1_1pacm_1_1InstallOptions) | [Package](#structscy_1_1pacm_1_1Package) installation options. |
| [`Package`](#structscy_1_1pacm_1_1Package) | This class is a JSON representation of an package belonging to the [PackageManager](#classscy_1_1pacm_1_1PackageManager). |
| [`Asset`](#structscy_1_1pacm_1_1Package_1_1Asset) | This class represents a archived file asset containing files belonging to the parent package. |
| [`RemotePackage`](#structscy_1_1pacm_1_1RemotePackage) | This class is a JSON representation of an package existing on the remote server that may be downloaded and installed. |
| [`LocalPackage`](#structscy_1_1pacm_1_1LocalPackage) | This class is a JSON representation of an installed local package that exists on the file system. |
| [`Manifest`](#structscy_1_1pacm_1_1LocalPackage_1_1Manifest) | This class provides a list of all package files and their location on the file system. |
| [`PackagePair`](#structscy_1_1pacm_1_1PackagePair) | This class provides pairing of a local and a remote package. |
| [`Options`](#structscy_1_1pacm_1_1PackageManager_1_1Options) | [Package](#structscy_1_1pacm_1_1Package) manager initialization options. |

### Members

| Name | Description |
|------|-------------|
| [`DEFAULT_API_ENDPOINT`](#group__pacm_1ga522de8a765e3f326637b19946ea8084a) |  |
| [`DEFAULT_API_INDEX_URI`](#group__pacm_1ga8a99b6b1f2c06d69f72b70a065e3abc3) |  |
| [`DEFAULT_PACKAGE_INSTALL_DIR`](#group__pacm_1ga605f59cf1dcb1a0b86673ae28f3765e8) |  |
| [`DEFAULT_PACKAGE_DATA_DIR`](#group__pacm_1ga3326a058ef7a92e406218251ed6548bd) |  |
| [`DEFAULT_PACKAGE_TEMP_DIR`](#group__pacm_1ga5a8319c27335e60523a50a50f2816f67) |  |
| [`DEFAULT_CHECKSUM_ALGORITHM`](#group__pacm_1ga1ea3bd19c5295bd7d1e6f6a862ef8b5d) |  |
| [`Pacm_API`](#group__pacm_1ga4e79c10778b27ed9c9f018f88560af1c) |  |

---

#### DEFAULT_API_ENDPOINT 

```cpp
DEFAULT_API_ENDPOINT()
```

---

#### DEFAULT_API_INDEX_URI 

```cpp
DEFAULT_API_INDEX_URI()
```

---

#### DEFAULT_PACKAGE_INSTALL_DIR 

```cpp
DEFAULT_PACKAGE_INSTALL_DIR()
```

---

#### DEFAULT_PACKAGE_DATA_DIR 

```cpp
DEFAULT_PACKAGE_DATA_DIR()
```

---

#### DEFAULT_PACKAGE_TEMP_DIR 

```cpp
DEFAULT_PACKAGE_TEMP_DIR()
```

---

#### DEFAULT_CHECKSUM_ALGORITHM 

```cpp
DEFAULT_CHECKSUM_ALGORITHM()
```

---

#### Pacm_API 

```cpp
Pacm_API()
```

## InstallMonitor 

> **Defined in:** `installmonitor.h`

### Members

| Name | Description |
|------|-------------|
| [`InstallMonitor`](#group__pacm_1ga0120805da5155e36bf6be738d58f1550) |  |
| [`~InstallMonitor`](#group__pacm_1ga9029008260c1d966e942f2b720d2237c) |  |
| [`addTask`](#group__pacm_1gade82e15d697cebdcf4c27c79fba73d7b) | Adds a task to monitor. |
| [`startAll`](#group__pacm_1gaa77ef7964ec12fd63329648d615588f7) | Starts all monitored tasks. |
| [`cancelAll`](#group__pacm_1ga188395007e67e4562b350ebc945c347a) | Cancels all monitored tasks. |
| [`isComplete`](#group__pacm_1ga7eb6c0cd1bcb8ed8b38451a38b93a818) | Returns true if all install tasks have completed, either successfully or unsuccessfully. |
| [`tasks`](#group__pacm_1ga25a15551d09c6b50c27a628f9f548ed6) | Returns the list of monitored package tasks. |
| [`packages`](#group__pacm_1gae85037281be09f4c389ca0a1210d4793) | Returns the list of monitored packages. |
| [`onInstallStateChange`](#group__pacm_1gaa8d8ae7796e966977d7ccc052e7e392e) |  |
| [`onInstallComplete`](#group__pacm_1ga3fee590aa5c73808d0f36de1c70f5b0d) |  |
| [`setProgress`](#group__pacm_1ga747222082b89e98e0d6befc96c1bcc16) |  |
| [`InstallStateChange`](#group__pacm_1ga5f7cb24385268ab5d2b50da72730d10a) | Proxies state change events from managed packages. |
| [`InstallComplete`](#group__pacm_1ga081f4bd707b531dfd4d38f2233588bf8) | Signals when a managed install task completes. |
| [`Progress`](#group__pacm_1ga293627c638d0d1f70e82546be766219a) | Signals on overall progress update [0-100]. |
| [`Complete`](#group__pacm_1ga9bdc4dad951bb031a861bb53efa411b4) | Signals on all tasks complete. |
| [`_mutex`](#group__pacm_1ga62930097be146c2422b7057622cd7ca4) |  |
| [`_tasks`](#group__pacm_1ga9466e58faabe5386ae0e8cc4b053baf0) |  |
| [`_packages`](#group__pacm_1ga5c5266c23cb36a98bbaf513f3c58beab) |  |
| [`_progress`](#group__pacm_1ga3cfe9db2c67679216a9f84d31060c9c9) |  |

---

#### InstallMonitor 

```cpp
InstallMonitor()
```

---

#### ~InstallMonitor 

```cpp
virtual ~InstallMonitor()
```

---

#### addTask 

```cpp
virtual void addTask(InstallTask::Ptr task)
```

Adds a task to monitor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `InstallTask::Ptr` |  |

---

#### startAll 

```cpp
virtual void startAll()
```

Starts all monitored tasks.

---

#### cancelAll 

```cpp
virtual void cancelAll()
```

Cancels all monitored tasks.

---

#### isComplete 

```cpp
virtual bool isComplete() const
```

Returns true if all install tasks have completed, either successfully or unsuccessfully.

---

#### tasks 

```cpp
virtual InstallTaskPtrVec tasks() const
```

Returns the list of monitored package tasks.

---

#### packages 

```cpp
virtual LocalPackageVec packages() const
```

Returns the list of monitored packages.

---

#### onInstallStateChange 

```cpp
virtual void onInstallStateChange(void * sender, InstallationState & state, const InstallationState & oldState)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `state` | `InstallationState &` |  |
| `oldState` | `const InstallationState &` |  |

---

#### onInstallComplete 

```cpp
virtual void onInstallComplete(InstallTask & task)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `InstallTask &` |  |

---

#### setProgress 

```cpp
virtual void setProgress(int value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `int` |  |

---

#### InstallStateChange 

```cpp
Signal< void(InstallTask &, const InstallationState &, const InstallationState &)> InstallStateChange
```

Proxies state change events from managed packages.

---

#### InstallComplete 

```cpp
Signal< void(LocalPackage &)> InstallComplete
```

Signals when a managed install task completes.

---

#### Progress 

```cpp
Signal< void(int &)> Progress
```

Signals on overall progress update [0-100].

---

#### Complete 

```cpp
Signal< void(LocalPackageVec &)> Complete
```

Signals on all tasks complete.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _tasks 

```cpp
InstallTaskPtrVec _tasks
```

---

#### _packages 

```cpp
LocalPackageVec _packages
```

---

#### _progress 

```cpp
int _progress
```

## InstallTask 

> **Extends:** `scy::basic::Runnable`, `scy::Stateful< InstallationState >`
> **Defined in:** `installtask.h`

This class implements the package installation procedure.

### Members

| Name | Description |
|------|-------------|
| [`InstallTask`](#group__pacm_1gac517d62628a6206362e8f8b8e8b6889e) |  |
| [`~InstallTask`](#group__pacm_1ga136d640d67a5bab3c7a2c4a5607bd6db) |  |
| [`start`](#group__pacm_1ga26a08fe537a7db788079445921650685) |  |
| [`cancel`](#group__pacm_1ga8847a1e10771443fa49ea35939a22567) |  |
| [`doDownload`](#group__pacm_1gaf815fbda71ebf69e76e015b68991e7b4) | Downloads the package archive from the server. |
| [`doExtract`](#group__pacm_1ga2f72241766459b160c804adc1478f5de) | Extracts the downloaded package files to the intermediate directory. |
| [`doFinalize`](#group__pacm_1ga6a1835d063d88174640ef5a28f883ac8) | Moves extracted files from the intermediate directory to the installation directory. |
| [`setComplete`](#group__pacm_1ga26d9a2a7502b8b85c7ee3a925567d882) | Called when the task completes either successfully or in error. This will trigger destruction. |
| [`getRemoteAsset`](#group__pacm_1gac019583f25b3acdb49d9341c1a7978c6) |  |
| [`local`](#group__pacm_1gadbcbd1e53c273436d7115a41ed1873f7) |  |
| [`remote`](#group__pacm_1gaf63f6840681ee74ee5664c82ffb889a2) |  |
| [`options`](#group__pacm_1gadf2774d8799a8fd865fc4f6b73a00ed1) |  |
| [`loop`](#group__pacm_1ga682298509ac94f66851b7760ce672c57) |  |
| [`valid`](#group__pacm_1ga0a49aa89c7ace046190a1920c8db2704) |  |
| [`cancelled`](#group__pacm_1gad4433a04ac1bb9dd5a6d2f222df77418) | Returns true when the task has been cancelled. |
| [`failed`](#group__pacm_1ga98d6a383a58aab12cf84e2591774571f) |  |
| [`success`](#group__pacm_1ga10c6401c60f22f2845c9560e58496907) |  |
| [`complete`](#group__pacm_1ga0dab536f7d0d8eddcb0cc5c9bddd1a87) |  |
| [`progress`](#group__pacm_1ga3dbca4f9b48fa5266333f7428e45338b) |  |
| [`run`](#group__pacm_1gab2791d8750f5bf0deece4e7fd708a0c9) | Called asynchronously by the thread to do the work. |
| [`onStateChange`](#group__pacm_1ga2f29805629456fceb726bd8c7f03c253) |  |
| [`onDownloadProgress`](#group__pacm_1ga5a9f8befa6682fc5b725bab29d9cf60f) |  |
| [`onDownloadComplete`](#group__pacm_1ga9c58cacb35a5d15d143c329b404e2b76) |  |
| [`setProgress`](#group__pacm_1gaaa96e157962a5c8bfb70a84f5a49d634) |  |
| [`Progress`](#group__pacm_1gaf703cf11a21b644acc7e59c1ebf26485) | Signals on progress update [0-100]. |
| [`Complete`](#group__pacm_1gaccf8591e8208940a4ad7c8133e3ec0ad) | Signals on task completion for both success and failure cases. |
| [`_mutex`](#group__pacm_1ga87292e561d64eca2a4b2d6677eb87d15) |  |
| [`_runner`](#group__pacm_1gaf178666e39b153b95d05945e47602099) |  |
| [`_error`](#group__pacm_1gacb7051d1db8a70b40f24cfee662aa18f) |  |
| [`_manager`](#group__pacm_1ga75bf291aa5b026cc178ba1361d44ec92) |  |
| [`_local`](#group__pacm_1gaed93e1d4951c6672b7f2ce7a91469cfd) |  |
| [`_remote`](#group__pacm_1ga39c422153b97c6aa95a1b2fe74a4c0b5) |  |
| [`_options`](#group__pacm_1ga3d6b1dbf5dc37e7aaf8d012f37d2c2cf) |  |
| [`_progress`](#group__pacm_1ga969d6a197cb58d27f056a989d413057d) |  |
| [`_downloading`](#group__pacm_1gac770bceb041aaa041c79f05a03a290b9) |  |
| [`_dlconn`](#group__pacm_1gaa286e79832b53a9a13ff50c6db6166d3) |  |
| [`_loop`](#group__pacm_1gab2e5dc630f6211673263d278d29be932) |  |

---

#### InstallTask 

```cpp
InstallTask(PackageManager & manager, LocalPackage * local, RemotePackage * remote, const InstallOptions & options, uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `manager` | `PackageManager &` |  |
| `local` | `LocalPackage *` |  |
| `remote` | `RemotePackage *` |  |
| `options` | `const InstallOptions &` |  |
| `loop` | `uv::Loop *` |  |

---

#### ~InstallTask 

```cpp
virtual ~InstallTask()
```

---

#### start 

```cpp
virtual void start()
```

---

#### cancel 

```cpp
virtual void cancel()
```

---

#### doDownload 

```cpp
virtual void doDownload()
```

Downloads the package archive from the server.

---

#### doExtract 

```cpp
virtual void doExtract()
```

Extracts the downloaded package files to the intermediate directory.

---

#### doFinalize 

```cpp
virtual void doFinalize()
```

Moves extracted files from the intermediate directory to the installation directory.

---

#### setComplete 

```cpp
virtual void setComplete()
```

Called when the task completes either successfully or in error. This will trigger destruction.

---

#### getRemoteAsset 

```cpp
virtual Package::Asset getRemoteAsset() const
```

---

#### local 

```cpp
virtual LocalPackage * local() const
```

---

#### remote 

```cpp
virtual RemotePackage * remote() const
```

---

#### options 

```cpp
virtual InstallOptions & options()
```

---

#### loop 

```cpp
virtual uv::Loop * loop() const
```

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### cancelled 

```cpp
virtual bool cancelled() const
```

Returns true when the task has been cancelled.

---

#### failed 

```cpp
virtual bool failed() const
```

---

#### success 

```cpp
virtual bool success() const
```

---

#### complete 

```cpp
virtual bool complete() const
```

---

#### progress 

```cpp
virtual int progress() const
```

---

#### run 

```cpp
virtual void run()
```

Called asynchronously by the thread to do the work.

---

#### onStateChange 

```cpp
virtual void onStateChange(InstallationState & state, const InstallationState & oldState)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `InstallationState &` |  |
| `oldState` | `const InstallationState &` |  |

---

#### onDownloadProgress 

```cpp
virtual void onDownloadProgress(const double & progress)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `progress` | `const double &` |  |

---

#### onDownloadComplete 

```cpp
virtual void onDownloadComplete(const http::Response & response)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `response` | `const http::Response &` |  |

---

#### setProgress 

```cpp
virtual void setProgress(int value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `int` |  |

---

#### Progress 

```cpp
Signal< void(InstallTask &, int &)> Progress
```

Signals on progress update [0-100].

---

#### Complete 

```cpp
Signal< void(InstallTask &)> Complete
```

Signals on task completion for both success and failure cases.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _runner 

```cpp
Idler _runner
```

---

#### _error 

```cpp
[scy::Error](./doc/api-base.md#structscy_1_1Error) _error
```

---

#### _manager 

```cpp
PackageManager & _manager
```

---

#### _local 

```cpp
LocalPackage * _local
```

---

#### _remote 

```cpp
RemotePackage * _remote
```

---

#### _options 

```cpp
InstallOptions _options
```

---

#### _progress 

```cpp
int _progress
```

---

#### _downloading 

```cpp
bool _downloading
```

---

#### _dlconn 

```cpp
http::ClientConnection::Ptr _dlconn
```

---

#### _loop 

```cpp
uv::Loop * _loop
```

## PackageManager 

> **Defined in:** `packagemanager.h`

The [Package](#structscy_1_1pacm_1_1Package) Manager provides an interface for managing, installing, updating and uninstalling Pacm packages.

### Members

| Name | Description |
|------|-------------|
| [`PackageManager`](#group__pacm_1ga0294b3732c8c91eae3066bf22088dd8f) |  |
| [`~PackageManager`](#group__pacm_1ga0cc70a4263385dff8736a9e93d1e8e91) |  |
| [`initialize`](#group__pacm_1gad2aa7211618560cec5cadf2d2b935e3f) | Initialization Methods. |
| [`uninitialize`](#group__pacm_1ga3f0734d6e0baf127459b596be8f69041) |  |
| [`initialized`](#group__pacm_1ga35d5d38a71defe859770d1f5ea45dc29) |  |
| [`createDirectories`](#group__pacm_1ga7eac61590241621a6945610c642bbd20) | Creates the package manager directory structure if it does not already exist. |
| [`queryRemotePackages`](#group__pacm_1ga7a6a87813079f346cf8b1248ef6f954e) | Queries the server for a list of available packages. |
| [`loadLocalPackages`](#group__pacm_1ga274d4bdc0fb2824165912e29eaa4a83a) | Loads all local package manifests from file system. Clears all in memory package manifests. |
| [`loadLocalPackages`](#group__pacm_1gac57d2e0df47bd611aadd0018a8562bf5) | Loads all local package manifests residing the the given directory. This method may be called multiple times for different paths because it does not clear in memory package manifests. |
| [`saveLocalPackages`](#group__pacm_1ga0dd3c90a7e9f85b20e6c0e44484be0af) |  |
| [`saveLocalPackage`](#group__pacm_1gaf7b9979bcb8cb9cb83f7ad1cb435879c) | Saves the local package manifest to the file system. |
| [`parseRemotePackages`](#group__pacm_1ga5a84f69090718da9af13247e64e28156) | Parse the remote packages from the given JSON data string. |
| [`installPackage`](#group__pacm_1gad295633f0783292623a120b227685090) | [Package](#structscy_1_1pacm_1_1Package) Installation Methods. |
| [`installPackages`](#group__pacm_1ga606a44cfed11394c05485adc351e9c04) | Installs multiple packages. The same options will be passed to each task. If a [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#classscy_1_1pacm_1_1PackageManager) does not take ownership of the [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor). |
| [`updatePackage`](#group__pacm_1ga027124b092208da323cfc62299d9d4bc) | Updates a single package. Throws an exception if the package does not exist. The returned [InstallTask](#classscy_1_1pacm_1_1InstallTask) must be started. |
| [`updatePackages`](#group__pacm_1gac48da691471662ce57f5815ac9c91ab0) | Updates multiple packages. Throws an exception if the package does not exist. If a [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#classscy_1_1pacm_1_1PackageManager) does not take ownership of the [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor). |
| [`updateAllPackages`](#group__pacm_1gaabc1315c35c2204028ef39ef77201d33) | Updates all installed packages. |
| [`uninstallPackages`](#group__pacm_1ga21634eceac90c02a342e413eb444a9d1) | Uninstalls multiple packages. |
| [`uninstallPackage`](#group__pacm_1ga06e4a2f271745f6ea4c50d4a85953bd5) | Uninstalls a single package. |
| [`hasUnfinalizedPackages`](#group__pacm_1ga736cd4d51f8c5e39dcfa2159fcfa0c8e) | Returns true if there are updates available that have not yet been finalized. Packages may be unfinalized if there were files in use at the time of installation. |
| [`finalizeInstallations`](#group__pacm_1ga65f16318834b79ddfa651fbbee66b301) | Finalizes active installations by moving all package files to their target destination. If files are to be overwritten they must not be in use or finalization will fail. |
| [`getInstallTask`](#group__pacm_1gad2d2c89ce41a9e620bdb0abfb005c369) | [Task](./doc/api-base.md#classscy_1_1Task) Helper Methods. |
| [`tasks`](#group__pacm_1ga06761269e2391f30c5e0564156af8374) | Returns a list of all tasks. |
| [`cancelAllTasks`](#group__pacm_1ga86a2bcebdc5c0bd217877ff180f1640b) | Aborts all package installation tasks. All tasks must be aborted before clearing local or remote manifests. |
| [`getPackagePairs`](#group__pacm_1ga11c411f6e676fb723561f8dc0c7bab4c) | [Package](#structscy_1_1pacm_1_1Package) Helper Methods. |
| [`getUpdatablePackagePairs`](#group__pacm_1ga8d2d399c321a02b82363387c25641aa0) | Returns a list of package pairs which may be updated. All pairs will have both local and remote package pointers, and the remote version will be newer than the local version. |
| [`getPackagePair`](#group__pacm_1ga2b2ef4158487fa1d82350994b5af38a9) | Returns a local and remote package pair. An exception will be thrown if either the local or remote packages aren't available or are invalid. |
| [`getOrCreatePackagePair`](#group__pacm_1gada7825aae6e2a54ee37332750686f67b) | Returns a local and remote package pair. If the local package doesn't exist it will be created from the remote package. If the remote package doesn't exist a NotFoundException will be thrown. |
| [`createInstallTask`](#group__pacm_1gafa07546ae7c88588169868deda5401ac) | Creates a package installation task for the given pair. |
| [`installedPackageVersion`](#group__pacm_1ga14871be8a910f944116f75c379352803) | Returns the version number of an installed package. Exceptions will be thrown if the package does not exist, or is not fully installed. |
| [`getLatestInstallableAsset`](#group__pacm_1ga5b0b61d38eb166ee35ae458d45bc3ea2) | Returns the best asset to install, or throws a descriptive exception if no updates are available, or if the package is already up-to-date. This method takes version and SDK locks into consideration. |
| [`hasAvailableUpdates`](#group__pacm_1ga994577213392b7452e0cc4f49893295a) | Returns true if there are updates available for this package, false otherwise. |
| [`clearCache`](#group__pacm_1gae83f81efc6fac83c6b1feff2ab5dcc9f) | File Helper Methods. |
| [`clearPackageCache`](#group__pacm_1gad57a7658f320091a47c9f592d34daf7f) | Clears a package archive from the local cache. |
| [`clearCacheFile`](#group__pacm_1ga547f44c7bb1c2307ac62b7a73aa9cb72) | Clears a file from the local cache. |
| [`hasCachedFile`](#group__pacm_1gabba22fcdb14b4ffe8c1bbaaed0845bf8) | Checks if a package archive exists in the local cache. |
| [`isSupportedFileType`](#group__pacm_1ga2b6d7aca72d07ffb066ed33126e33b00) | Checks if the file type is a supported package archive. |
| [`getCacheFilePath`](#group__pacm_1gaf98ef2e87f1508708c8d6387a4442fc2) | Returns the full path of the cached file if it exists, or an empty path if the file doesn't exist. |
| [`getPackageDataDir`](#group__pacm_1ga0e449180c70179434ff03226f8f612ca) | Returns the package data directory for the given package ID. |
| [`options`](#group__pacm_1ga2f058814ce88bd8f906fd1a0798ee3dd) | Accessors. |
| [`remotePackages`](#group__pacm_1ga666c75b4746425e5dd7c0dbbe52062ed) |  |
| [`localPackages`](#group__pacm_1ga632003da9cc5e262bdbc679ee4dd2c9a) |  |
| [`onPackageInstallComplete`](#group__pacm_1ga51dc83f2084498013b72c545d9ec5124) | Callbacks. |
| [`RemotePackageResponse`](#group__pacm_1gac9bfa675fc261bd38052ae17bae25356) | Events. |
| [`PackageUninstalled`](#group__pacm_1gab05d4bd29a2c2ecc7579e58f919d43c5) | Signals when a package is uninstalled. |
| [`InstallTaskCreated`](#group__pacm_1ga7fbc7057386ad4ec58af8223d56c7bef) | Signals when an installation task is created, before it is started. |
| [`InstallTaskComplete`](#group__pacm_1ga4e0d38796a84080bbf09756a553503c7) | Signals when a package installation tasks completes, either successfully or in error. |
| [`_mutex`](#group__pacm_1gae98d53378ea091de1bf8021d83163c49) |  |
| [`_localPackages`](#group__pacm_1ga1f7e142696d6a5cace81ff92036d5079) |  |
| [`_remotePackages`](#group__pacm_1ga8cc192fdcf6a3aafd15a9083504e37da) |  |
| [`_tasks`](#group__pacm_1ga86909426338d228d0a01de9a98332617) |  |
| [`_options`](#group__pacm_1ga6883f4d86ccd9d91baf44e737b4e3913) |  |

---

#### PackageManager 

```cpp
PackageManager(const Options & options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `const Options &` |  |

---

#### ~PackageManager 

```cpp
virtual ~PackageManager()
```

---

#### initialize 

```cpp
virtual void initialize()
```

Initialization Methods.

---

#### uninitialize 

```cpp
virtual void uninitialize()
```

---

#### initialized 

```cpp
virtual bool initialized() const
```

---

#### createDirectories 

```cpp
virtual void createDirectories()
```

Creates the package manager directory structure if it does not already exist.

---

#### queryRemotePackages 

```cpp
virtual void queryRemotePackages()
```

Queries the server for a list of available packages.

---

#### loadLocalPackages 

```cpp
virtual void loadLocalPackages()
```

Loads all local package manifests from file system. Clears all in memory package manifests.

---

#### loadLocalPackages 

```cpp
virtual void loadLocalPackages(const std::string & dir)
```

Loads all local package manifests residing the the given directory. This method may be called multiple times for different paths because it does not clear in memory package manifests.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dir` | `const std::string &` |  |

---

#### saveLocalPackages 

```cpp
virtual bool saveLocalPackages(bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `whiny` | `bool` |  |

---

#### saveLocalPackage 

```cpp
virtual bool saveLocalPackage(LocalPackage & package, bool whiny)
```

Saves the local package manifest to the file system.

| Parameter | Type | Description |
|-----------|------|-------------|
| `package` | `LocalPackage &` |  |
| `whiny` | `bool` |  |

---

#### parseRemotePackages 

```cpp
virtual void parseRemotePackages(const std::string & data)
```

Parse the remote packages from the given JSON data string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const std::string &` |  |

---

#### installPackage 

```cpp
virtual InstallTask::Ptr installPackage(const std::string & name, const InstallOptions & options)
```

[Package](#structscy_1_1pacm_1_1Package) Installation Methods.

Installs a single package. The returned [InstallTask](#classscy_1_1pacm_1_1InstallTask) must be started. If the package is already up-to-date, a nullptr will be returned. Any other error will throw a std::runtime_error.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `options` | `const InstallOptions &` |  |

---

#### installPackages 

```cpp
virtual bool installPackages(const StringVec & ids, const InstallOptions & options, InstallMonitor * monitor, bool whiny)
```

Installs multiple packages. The same options will be passed to each task. If a [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#classscy_1_1pacm_1_1PackageManager) does not take ownership of the [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor).

| Parameter | Type | Description |
|-----------|------|-------------|
| `ids` | `const StringVec &` |  |
| `options` | `const InstallOptions &` |  |
| `monitor` | `InstallMonitor *` |  |
| `whiny` | `bool` |  |

---

#### updatePackage 

```cpp
virtual InstallTask::Ptr updatePackage(const std::string & name, const InstallOptions & options)
```

Updates a single package. Throws an exception if the package does not exist. The returned [InstallTask](#classscy_1_1pacm_1_1InstallTask) must be started.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `options` | `const InstallOptions &` |  |

---

#### updatePackages 

```cpp
virtual bool updatePackages(const StringVec & ids, const InstallOptions & options, InstallMonitor * monitor, bool whiny)
```

Updates multiple packages. Throws an exception if the package does not exist. If a [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor) instance was passed in the tasks will need to be started, otherwise they will be auto-started. The [PackageManager](#classscy_1_1pacm_1_1PackageManager) does not take ownership of the [InstallMonitor](#classscy_1_1pacm_1_1InstallMonitor).

| Parameter | Type | Description |
|-----------|------|-------------|
| `ids` | `const StringVec &` |  |
| `options` | `const InstallOptions &` |  |
| `monitor` | `InstallMonitor *` |  |
| `whiny` | `bool` |  |

---

#### updateAllPackages 

```cpp
virtual bool updateAllPackages(bool whiny)
```

Updates all installed packages.

| Parameter | Type | Description |
|-----------|------|-------------|
| `whiny` | `bool` |  |

---

#### uninstallPackages 

```cpp
virtual bool uninstallPackages(const StringVec & ids, bool whiny)
```

Uninstalls multiple packages.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ids` | `const StringVec &` |  |
| `whiny` | `bool` |  |

---

#### uninstallPackage 

```cpp
virtual bool uninstallPackage(const std::string & id, bool whiny)
```

Uninstalls a single package.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### hasUnfinalizedPackages 

```cpp
virtual bool hasUnfinalizedPackages()
```

Returns true if there are updates available that have not yet been finalized. Packages may be unfinalized if there were files in use at the time of installation.

---

#### finalizeInstallations 

```cpp
virtual bool finalizeInstallations(bool whiny)
```

Finalizes active installations by moving all package files to their target destination. If files are to be overwritten they must not be in use or finalization will fail.

| Parameter | Type | Description |
|-----------|------|-------------|
| `whiny` | `bool` |  |

---

#### getInstallTask 

```cpp
virtual InstallTask::Ptr getInstallTask(const std::string & id) const
```

[Task](./doc/api-base.md#classscy_1_1Task) Helper Methods.

Gets the install task for the given package ID.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### tasks 

```cpp
virtual InstallTaskPtrVec tasks() const
```

Returns a list of all tasks.

---

#### cancelAllTasks 

```cpp
virtual void cancelAllTasks()
```

Aborts all package installation tasks. All tasks must be aborted before clearing local or remote manifests.

---

#### getPackagePairs 

```cpp
virtual PackagePairVec getPackagePairs() const
```

[Package](#structscy_1_1pacm_1_1Package) Helper Methods.

Returns all package pairs, valid or invalid. Some pairs may not have both local and remote package pointers.

---

#### getUpdatablePackagePairs 

```cpp
virtual PackagePairVec getUpdatablePackagePairs() const
```

Returns a list of package pairs which may be updated. All pairs will have both local and remote package pointers, and the remote version will be newer than the local version.

---

#### getPackagePair 

```cpp
virtual PackagePair getPackagePair(const std::string & id, bool whiny) const
```

Returns a local and remote package pair. An exception will be thrown if either the local or remote packages aren't available or are invalid.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### getOrCreatePackagePair 

```cpp
virtual PackagePair getOrCreatePackagePair(const std::string & id)
```

Returns a local and remote package pair. If the local package doesn't exist it will be created from the remote package. If the remote package doesn't exist a NotFoundException will be thrown.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### createInstallTask 

```cpp
virtual InstallTask::Ptr createInstallTask(PackagePair & pair, const InstallOptions & options)
```

Creates a package installation task for the given pair.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pair` | `PackagePair &` |  |
| `options` | `const InstallOptions &` |  |

---

#### installedPackageVersion 

```cpp
virtual std::string installedPackageVersion(const std::string & id) const
```

Returns the version number of an installed package. Exceptions will be thrown if the package does not exist, or is not fully installed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### getLatestInstallableAsset 

```cpp
virtual Package::Asset getLatestInstallableAsset(const PackagePair & pair, const InstallOptions & options) const
```

Returns the best asset to install, or throws a descriptive exception if no updates are available, or if the package is already up-to-date. This method takes version and SDK locks into consideration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pair` | `const PackagePair &` |  |
| `options` | `const InstallOptions &` |  |

---

#### hasAvailableUpdates 

```cpp
virtual bool hasAvailableUpdates(const PackagePair & pair) const
```

Returns true if there are updates available for this package, false otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `pair` | `const PackagePair &` |  |

---

#### clearCache 

```cpp
void clearCache()
```

File Helper Methods.

Clears all files in the cache directory.

---

#### clearPackageCache 

```cpp
bool clearPackageCache(LocalPackage & package)
```

Clears a package archive from the local cache.

| Parameter | Type | Description |
|-----------|------|-------------|
| `package` | `LocalPackage &` |  |

---

#### clearCacheFile 

```cpp
bool clearCacheFile(const std::string & fileName, bool whiny)
```

Clears a file from the local cache.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fileName` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### hasCachedFile 

```cpp
bool hasCachedFile(Package::Asset & asset)
```

Checks if a package archive exists in the local cache.

| Parameter | Type | Description |
|-----------|------|-------------|
| `asset` | `Package::Asset &` |  |

---

#### isSupportedFileType 

```cpp
bool isSupportedFileType(const std::string & fileName)
```

Checks if the file type is a supported package archive.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fileName` | `const std::string &` |  |

---

#### getCacheFilePath 

```cpp
std::string getCacheFilePath(const std::string & fileName)
```

Returns the full path of the cached file if it exists, or an empty path if the file doesn't exist.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fileName` | `const std::string &` |  |

---

#### getPackageDataDir 

```cpp
std::string getPackageDataDir(const std::string & id)
```

Returns the package data directory for the given package ID.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### options 

```cpp
virtual Options & options()
```

Accessors.

---

#### remotePackages 

```cpp
virtual RemotePackageStore & remotePackages()
```

---

#### localPackages 

```cpp
virtual LocalPackageStore & localPackages()
```

---

#### onPackageInstallComplete 

```cpp
void onPackageInstallComplete(InstallTask & task)
```

Callbacks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `InstallTask &` |  |

---

#### RemotePackageResponse 

```cpp
Signal< void(const http::Response &)> RemotePackageResponse
```

Events.

Signals when the remote package list have been downloaded from the server.

---

#### PackageUninstalled 

```cpp
Signal< void(LocalPackage &)> PackageUninstalled
```

Signals when a package is uninstalled.

---

#### InstallTaskCreated 

```cpp
Signal< void(InstallTask &)> InstallTaskCreated
```

Signals when an installation task is created, before it is started.

---

#### InstallTaskComplete 

```cpp
Signal< void(const InstallTask &)> InstallTaskComplete
```

Signals when a package installation tasks completes, either successfully or in error.

---

#### _mutex 

```cpp
std::mutex _mutex
```

---

#### _localPackages 

```cpp
LocalPackageStore _localPackages
```

---

#### _remotePackages 

```cpp
RemotePackageStore _remotePackages
```

---

#### _tasks 

```cpp
InstallTaskPtrVec _tasks
```

---

#### _options 

```cpp
Options _options
```

## Options 

> **Defined in:** `packagemanager.h`

[Package](#structscy_1_1pacm_1_1Package) manager initialization options.

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__pacm_1ga5bf57541a7fc15cae0283c80543060fb) |  |
| [`endpoint`](#group__pacm_1ga17eaf31ffc1fc663c74715c0861feaa7) | The HTTP server endpoint. |
| [`indexURI`](#group__pacm_1gaeaf07e705ed06b76a6e6da069b06933a) | The HTTP server URI for querying packages JSON. |
| [`httpUsername`](#group__pacm_1ga2d4089adeb8ea1b332289aa912f9670d) | Username for HTTP basic auth. |
| [`httpPassword`](#group__pacm_1gad72b21a0fd0afc7d0afbe79a267977c9) | PAssword for HTTP basic auth. |
| [`httpOAuthToken`](#group__pacm_1ga3ae63176c907fe93eafb52451c55aa5d) | Will be used instead of HTTP basic if provided. |
| [`tempDir`](#group__pacm_1ga9baae9668ea2afb09d479bc4e1622059) | Directory where package files will be downloaded and extracted. |
| [`dataDir`](#group__pacm_1ga2983a9e79dab4c044b56a53c3bec3ba0) | Directory where package manifests will be kept. |
| [`installDir`](#group__pacm_1ga18001156609c5fe9845f4d0c259964ff) | Directory where packages will be installed. |
| [`platform`](#group__pacm_1gaa9e36a87975600551306e8551065414b) | Platform (win32, linux, mac) |
| [`checksumAlgorithm`](#group__pacm_1gaa458d98644fdde252a9562c01028ad47) | Checksum algorithm (MDS/SHA1) |
| [`clearFailedCache`](#group__pacm_1ga791221a28415eff06ee9a162e950e9dd) | This flag tells the package manager weather or not to clear the package cache if installation fails. |

---

#### Options 

```cpp
inline Options(const std::string & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const std::string &` |  |

---

#### endpoint 

```cpp
std::string endpoint
```

The HTTP server endpoint.

---

#### indexURI 

```cpp
std::string indexURI
```

The HTTP server URI for querying packages JSON.

---

#### httpUsername 

```cpp
std::string httpUsername
```

Username for HTTP basic auth.

---

#### httpPassword 

```cpp
std::string httpPassword
```

PAssword for HTTP basic auth.

---

#### httpOAuthToken 

```cpp
std::string httpOAuthToken
```

Will be used instead of HTTP basic if provided.

---

#### tempDir 

```cpp
std::string tempDir
```

Directory where package files will be downloaded and extracted.

---

#### dataDir 

```cpp
std::string dataDir
```

Directory where package manifests will be kept.

---

#### installDir 

```cpp
std::string installDir
```

Directory where packages will be installed.

---

#### platform 

```cpp
std::string platform
```

Platform (win32, linux, mac)

---

#### checksumAlgorithm 

```cpp
std::string checksumAlgorithm
```

Checksum algorithm (MDS/SHA1)

---

#### clearFailedCache 

```cpp
bool clearFailedCache
```

This flag tells the package manager weather or not to clear the package cache if installation fails.

## InstallationState 

> **Extends:** `scy::State`
> **Defined in:** `installtask.h`

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__pacm_1ga84f70fa8f6abb28bab6a1c4e15bea7fa) |  |
| [`str`](#group__pacm_1gaa40b0c6a943b11ea07aa7464d60804d2) |  |

---

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

---

#### str 

```cpp
inline std::string str(unsigned int id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `unsigned int` |  |

## InstallOptions 

> **Defined in:** `installtask.h`

[Package](#structscy_1_1pacm_1_1Package) installation options.

### Members

| Name | Description |
|------|-------------|
| [`InstallOptions`](#group__pacm_1ga948d1e6960c54926cf870ac7e6aef7ab) |  |
| [`version`](#group__pacm_1ga9042e51b870e079ca083c96bedf53a49) | If set then the given package version will be installed. |
| [`sdkVersion`](#group__pacm_1ga179a2d6514580071b30f8b4f87b77b3a) | If set then the latest package version for given SDK version will be installed. |
| [`installDir`](#group__pacm_1gaa7c2627c783fa1ef3b919c40c8c51acb) | Install to the given location, otherwise the manager default `installDir` will be used. |

---

#### InstallOptions 

```cpp
inline InstallOptions()
```

---

#### version 

```cpp
std::string version
```

If set then the given package version will be installed.

---

#### sdkVersion 

```cpp
std::string sdkVersion
```

If set then the latest package version for given SDK version will be installed.

---

#### installDir 

```cpp
std::string installDir
```

Install to the given location, otherwise the manager default `installDir` will be used.

## Package 

> **Extends:** `json::value`
> **Subclasses:** `scy::pacm::LocalPackage`, `scy::pacm::RemotePackage`
> **Defined in:** `package.h`

This class is a JSON representation of an package belonging to the [PackageManager](#classscy_1_1pacm_1_1PackageManager).

### Members

| Name | Description |
|------|-------------|
| [`Package`](#group__pacm_1ga2703eff38205e8c4fc502029c6666c31) |  |
| [`Package`](#group__pacm_1ga0bb20d604c0310471e51c534350e234b) |  |
| [`~Package`](#group__pacm_1ga33db4f8d2056f384b0c9797fa3ae35c0) |  |
| [`id`](#group__pacm_1gadf03f04052d025ba84db73312dea23bb) |  |
| [`name`](#group__pacm_1ga7fc35ed22e76fe0e4dc8e42ed82e5f3c) |  |
| [`type`](#group__pacm_1gaf6e00584f5cc5ed044cd28e55f87ccea) |  |
| [`author`](#group__pacm_1gad4b921e4c2f96f90b9762edfec3ad752) |  |
| [`description`](#group__pacm_1ga6466be90a103884930f146af5acdc1a2) |  |
| [`valid`](#group__pacm_1gab539f78ec4bc6fceb3944dc34936ec6c) |  |
| [`print`](#group__pacm_1gaa2fc0fb484ee99c87f2997b3f8a3fe6b) |  |

---

#### Package 

```cpp
Package()
```

---

#### Package 

```cpp
Package(const json::value & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `const json::value &` |  |

---

#### ~Package 

```cpp
virtual ~Package()
```

---

#### id 

```cpp
virtual std::string id() const
```

---

#### name 

```cpp
virtual std::string name() const
```

---

#### type 

```cpp
virtual std::string type() const
```

---

#### author 

```cpp
virtual std::string author() const
```

---

#### description 

```cpp
virtual std::string description() const
```

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

## Asset 

> **Defined in:** `package.h`

This class represents a archived file asset containing files belonging to the parent package.

### Members

| Name | Description |
|------|-------------|
| [`Asset`](#group__pacm_1ga29d6d991aff1425c826b710f9136f4c8) |  |
| [`~Asset`](#group__pacm_1ga9555259d3bcd6750e24f1c5cef5bd441) |  |
| [`fileName`](#group__pacm_1ga9a79ee260434ba1b30489d1d6e9dd421) |  |
| [`version`](#group__pacm_1ga6f83300447f0b5c1f7c4ec43cba3a5da) |  |
| [`sdkVersion`](#group__pacm_1gabed743e99c221cc89bc4531db63b1e38) |  |
| [`checksum`](#group__pacm_1gab4c084cccb50ec5c9597cf441cfe5a39) |  |
| [`url`](#group__pacm_1gafbad31de39ae32ca71a513142d4b6ded) |  |
| [`fileSize`](#group__pacm_1ga6a460e9ddb8c78f258a5ad16f6ef07e6) |  |
| [`valid`](#group__pacm_1ga20d7328f063017efa5e8b5221361a51b) |  |
| [`print`](#group__pacm_1ga3d847d7913a955228cf0a4600e8e9452) |  |
| [`operator=`](#group__pacm_1gae5c60ffe2dd0ff7459182a5ba32ff8f8) |  |
| [`operator==`](#group__pacm_1ga11abf75c67f982309036a02d79d3084b) |  |
| [`root`](#group__pacm_1ga626d075e5da0403df011551bcbacda82) |  |

---

#### Asset 

```cpp
Asset(json::value & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `json::value &` |  |

---

#### ~Asset 

```cpp
virtual ~Asset()
```

---

#### fileName 

```cpp
virtual std::string fileName() const
```

---

#### version 

```cpp
virtual std::string version() const
```

---

#### sdkVersion 

```cpp
virtual std::string sdkVersion() const
```

---

#### checksum 

```cpp
virtual std::string checksum() const
```

---

#### url 

```cpp
virtual std::string url(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### fileSize 

```cpp
virtual int fileSize() const
```

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### operator= 

```cpp
virtual Asset & operator=(const Asset & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Asset &` |  |

---

#### operator== 

```cpp
virtual bool operator==(const Asset & r) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Asset &` |  |

---

#### root 

```cpp
json::value & root
```

## Asset 

> **Defined in:** `package.h`

This class represents a archived file asset containing files belonging to the parent package.

### Members

| Name | Description |
|------|-------------|
| [`Asset`](#group__pacm_1ga29d6d991aff1425c826b710f9136f4c8) |  |
| [`~Asset`](#group__pacm_1ga9555259d3bcd6750e24f1c5cef5bd441) |  |
| [`fileName`](#group__pacm_1ga9a79ee260434ba1b30489d1d6e9dd421) |  |
| [`version`](#group__pacm_1ga6f83300447f0b5c1f7c4ec43cba3a5da) |  |
| [`sdkVersion`](#group__pacm_1gabed743e99c221cc89bc4531db63b1e38) |  |
| [`checksum`](#group__pacm_1gab4c084cccb50ec5c9597cf441cfe5a39) |  |
| [`url`](#group__pacm_1gafbad31de39ae32ca71a513142d4b6ded) |  |
| [`fileSize`](#group__pacm_1ga6a460e9ddb8c78f258a5ad16f6ef07e6) |  |
| [`valid`](#group__pacm_1ga20d7328f063017efa5e8b5221361a51b) |  |
| [`print`](#group__pacm_1ga3d847d7913a955228cf0a4600e8e9452) |  |
| [`operator=`](#group__pacm_1gae5c60ffe2dd0ff7459182a5ba32ff8f8) |  |
| [`operator==`](#group__pacm_1ga11abf75c67f982309036a02d79d3084b) |  |
| [`root`](#group__pacm_1ga626d075e5da0403df011551bcbacda82) |  |

---

#### Asset 

```cpp
Asset(json::value & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `json::value &` |  |

---

#### ~Asset 

```cpp
virtual ~Asset()
```

---

#### fileName 

```cpp
virtual std::string fileName() const
```

---

#### version 

```cpp
virtual std::string version() const
```

---

#### sdkVersion 

```cpp
virtual std::string sdkVersion() const
```

---

#### checksum 

```cpp
virtual std::string checksum() const
```

---

#### url 

```cpp
virtual std::string url(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### fileSize 

```cpp
virtual int fileSize() const
```

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### print 

```cpp
virtual void print(std::ostream & ost) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### operator= 

```cpp
virtual Asset & operator=(const Asset & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Asset &` |  |

---

#### operator== 

```cpp
virtual bool operator==(const Asset & r) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const Asset &` |  |

---

#### root 

```cpp
json::value & root
```

## RemotePackage 

> **Extends:** `scy::pacm::Package`
> **Defined in:** `package.h`

This class is a JSON representation of an package existing on the remote server that may be downloaded and installed.

### Members

| Name | Description |
|------|-------------|
| [`RemotePackage`](#group__pacm_1gaf64c095ee7ff79f3b3222c1e31e3f9b9) |  |
| [`RemotePackage`](#group__pacm_1gaaa4c25d50d5f6c5207e2a66775149eca) |  |
| [`~RemotePackage`](#group__pacm_1ga40701168173d0770c14a9ab7cd48c5c6) |  |
| [`assets`](#group__pacm_1ga8bc111ca15a6078bae87e00336cb2145) |  |
| [`latestAsset`](#group__pacm_1ga78a9d3c1e3023826f3c39f701cbf76cd) | Returns the latest asset for this package. For local packages this is the currently installed version. For remote packages this is the latest available version. Throws an exception if no asset exists. |
| [`assetVersion`](#group__pacm_1ga2aecae77982bef0703bd05e3381a4396) | Returns the latest asset for the given package version. Throws an exception if no asset exists. |
| [`latestSDKAsset`](#group__pacm_1ga43652748df730786052a32cfbc212971) | Returns the latest asset for the given SDK version. This method is for safely installing plug-ins which must be compiled against a specific SDK version. The package JSON must have a "sdk-version" member for this function to work as intended. Throws an exception if no asset exists. |

---

#### RemotePackage 

```cpp
RemotePackage()
```

---

#### RemotePackage 

```cpp
RemotePackage(const json::value & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `const json::value &` |  |

---

#### ~RemotePackage 

```cpp
virtual ~RemotePackage()
```

---

#### assets 

```cpp
virtual json::value & assets()
```

---

#### latestAsset 

```cpp
virtual Asset latestAsset()
```

Returns the latest asset for this package. For local packages this is the currently installed version. For remote packages this is the latest available version. Throws an exception if no asset exists.

---

#### assetVersion 

```cpp
virtual Asset assetVersion(const std::string & version)
```

Returns the latest asset for the given package version. Throws an exception if no asset exists.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

#### latestSDKAsset 

```cpp
virtual Asset latestSDKAsset(const std::string & version)
```

Returns the latest asset for the given SDK version. This method is for safely installing plug-ins which must be compiled against a specific SDK version. The package JSON must have a "sdk-version" member for this function to work as intended. Throws an exception if no asset exists.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

## LocalPackage 

> **Extends:** `scy::pacm::Package`
> **Defined in:** `package.h`

This class is a JSON representation of an installed local package that exists on the file system.

### Members

| Name | Description |
|------|-------------|
| [`LocalPackage`](#group__pacm_1ga146358d8ea6e0742af85e8dd1ed3cfbe) |  |
| [`LocalPackage`](#group__pacm_1ga36942c874328ff874b5b558d2fc69adb) |  |
| [`LocalPackage`](#group__pacm_1ga8d915798d4874783e5f1a93460c8a790) | Create the local package from the remote package reference with the following manipulations. 1) Add a local manifest element. 2) Remove asset mirror elements. |
| [`~LocalPackage`](#group__pacm_1gae753b7b0b571a560586e8f9f5f04c22b) |  |
| [`setState`](#group__pacm_1gad38e4481bbac043f2bbcd7dd0cf23c15) | Set's the overall package state. Possible values are: Installing, Installed, Failed, Uninstalled. If the packages completes while still Installing, this means the package has yet to be finalized. |
| [`setInstallState`](#group__pacm_1ga43dfd9e6481e0d3f7e42767024db6051) | Set's the package installation state. See [InstallationState](#structscy_1_1pacm_1_1InstallationState) for possible values. |
| [`setInstallDir`](#group__pacm_1gad2f95cca3661df99a22d600cda6cf73d) | Set's the installation directory for this package. |
| [`setInstalledAsset`](#group__pacm_1ga2d16fefb83a30029394f13655f82999f) | Sets the installed asset, once installed. This method also sets the version. |
| [`setVersion`](#group__pacm_1ga1fe540ab674a6df4f1fe21beba27d0bb) | Sets the current version of the local package. Installation must be complete. |
| [`setVersionLock`](#group__pacm_1ga00f12d1314c1fe28733935ee118574f9) | Locks the package at the given version. Once set this package will not be updated past the given version. Pass an empty string to remove the lock. |
| [`setSDKVersionLock`](#group__pacm_1gadf546ce2b38e0afc4efddbf701fb573f) | Locks the package at the given SDK version. Once set this package will only update to the most recent version with given SDK version. Pass an empty string to remove the lock. |
| [`version`](#group__pacm_1gad30980d365279cfaee755cfe0561a170) | Returns the installed package version. |
| [`state`](#group__pacm_1ga4c5569e8655f1994146458e6f2cb13cf) | Returns the current state of this package. |
| [`installState`](#group__pacm_1ga5d9b98d55946300cd06145855241b13d) | Returns the installation state of this package. |
| [`installDir`](#group__pacm_1ga7f84ea33e777a251e2893a1f69fa31e3) | Returns the installation directory for this package. |
| [`versionLock`](#group__pacm_1ga750d04ba34e63bca0c4c5761f161cea9) |  |
| [`sdkLockedVersion`](#group__pacm_1ga0e32d29a0d4c5ac84f85e81393ed003f) |  |
| [`asset`](#group__pacm_1ga8bf8741c74d2cc37fa1e6731bebbf37f) | Returns the currently installed asset, if any. If none, the returned asset will be empty(). |
| [`isInstalled`](#group__pacm_1gac915b269b5ee73baf22c81a44a337e9d) | Returns true or false depending on weather or not the package is installed successfully. False if package is in Failed state. |
| [`isFailed`](#group__pacm_1ga7f0274050c627c6a040ff65cb75aa4ec) |  |
| [`manifest`](#group__pacm_1ga9ce8a07f05febda5e87b1a2a706af857) | Returns the installation manifest. |
| [`verifyInstallManifest`](#group__pacm_1gaf1a9c2bdd76d940f200c53e6e175a587) |  |
| [`getInstalledFilePath`](#group__pacm_1ga9f24a8d655179921947edd97ff2a4395) | Returns the full full path of the installed file. Thrown an exception if the install directory is unset. |
| [`errors`](#group__pacm_1ga068d92ded3a5a7997210227b7ed29e2a) |  |
| [`addError`](#group__pacm_1ga359b1844b6d513f8e7daa83ce2eb1467) |  |
| [`lastError`](#group__pacm_1gae1e11871cc82282a8ad592ba55fdf121) |  |
| [`clearErrors`](#group__pacm_1gafb51214393066dc3a75690fa5fff966e) |  |
| [`valid`](#group__pacm_1ga6778e88ac7cb3bdc1fba171e9cfe4e85) |  |

---

#### LocalPackage 

```cpp
LocalPackage()
```

---

#### LocalPackage 

```cpp
LocalPackage(const json::value & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `const json::value &` |  |

---

#### LocalPackage 

```cpp
LocalPackage(const RemotePackage & src)
```

Create the local package from the remote package reference with the following manipulations. 1) Add a local manifest element. 2) Remove asset mirror elements.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `const RemotePackage &` |  |

---

#### ~LocalPackage 

```cpp
virtual ~LocalPackage()
```

---

#### setState 

```cpp
virtual void setState(const std::string & state)
```

Set's the overall package state. Possible values are: Installing, Installed, Failed, Uninstalled. If the packages completes while still Installing, this means the package has yet to be finalized.

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `const std::string &` |  |

---

#### setInstallState 

```cpp
virtual void setInstallState(const std::string & state)
```

Set's the package installation state. See [InstallationState](#structscy_1_1pacm_1_1InstallationState) for possible values.

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `const std::string &` |  |

---

#### setInstallDir 

```cpp
virtual void setInstallDir(const std::string & dir)
```

Set's the installation directory for this package.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dir` | `const std::string &` |  |

---

#### setInstalledAsset 

```cpp
virtual void setInstalledAsset(const Package::Asset & installedRemoteAsset)
```

Sets the installed asset, once installed. This method also sets the version.

| Parameter | Type | Description |
|-----------|------|-------------|
| `installedRemoteAsset` | `const Package::Asset &` |  |

---

#### setVersion 

```cpp
virtual void setVersion(const std::string & version)
```

Sets the current version of the local package. Installation must be complete.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

#### setVersionLock 

```cpp
virtual void setVersionLock(const std::string & version)
```

Locks the package at the given version. Once set this package will not be updated past the given version. Pass an empty string to remove the lock.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

#### setSDKVersionLock 

```cpp
virtual void setSDKVersionLock(const std::string & version)
```

Locks the package at the given SDK version. Once set this package will only update to the most recent version with given SDK version. Pass an empty string to remove the lock.

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `const std::string &` |  |

---

#### version 

```cpp
virtual std::string version() const
```

Returns the installed package version.

---

#### state 

```cpp
virtual std::string state() const
```

Returns the current state of this package.

---

#### installState 

```cpp
virtual std::string installState() const
```

Returns the installation state of this package.

---

#### installDir 

```cpp
virtual std::string installDir() const
```

Returns the installation directory for this package.

---

#### versionLock 

```cpp
virtual std::string versionLock() const
```

---

#### sdkLockedVersion 

```cpp
virtual std::string sdkLockedVersion() const
```

---

#### asset 

```cpp
virtual Asset asset()
```

Returns the currently installed asset, if any. If none, the returned asset will be empty().

---

#### isInstalled 

```cpp
virtual bool isInstalled() const
```

Returns true or false depending on weather or not the package is installed successfully. False if package is in Failed state.

---

#### isFailed 

```cpp
virtual bool isFailed() const
```

---

#### manifest 

```cpp
virtual Manifest manifest()
```

Returns the installation manifest.

---

#### verifyInstallManifest 

```cpp
virtual bool verifyInstallManifest(bool allowEmpty)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `allowEmpty` | `bool` |  |

---

#### getInstalledFilePath 

```cpp
virtual std::string getInstalledFilePath(const std::string & fileName, bool whiny)
```

Returns the full full path of the installed file. Thrown an exception if the install directory is unset.

| Parameter | Type | Description |
|-----------|------|-------------|
| `fileName` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### errors 

```cpp
virtual json::value & errors()
```

---

#### addError 

```cpp
virtual void addError(const std::string & message)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `const std::string &` |  |

---

#### lastError 

```cpp
virtual std::string lastError() const
```

---

#### clearErrors 

```cpp
virtual void clearErrors()
```

---

#### valid 

```cpp
virtual bool valid() const
```

## Manifest 

> **Defined in:** `package.h`

This class provides a list of all package files and their location on the file system.

### Members

| Name | Description |
|------|-------------|
| [`Manifest`](#group__pacm_1ga27938a9ecf0a7d28a49a33fbf1cf24b8) |  |
| [`~Manifest`](#group__pacm_1ga9d7098ebc7d12fc1dbabd8c2d07aa05b) |  |
| [`empty`](#group__pacm_1gaa767187173b460a6ed903ad7b5237b87) |  |
| [`addFile`](#group__pacm_1ga46bf1459f6d73eb846287739d0b0c2f2) |  |
| [`root`](#group__pacm_1gaac9eb9eb02a577f627687939e65a3af7) |  |

---

#### Manifest 

```cpp
Manifest(json::value & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `json::value &` |  |

---

#### ~Manifest 

```cpp
virtual ~Manifest()
```

---

#### empty 

```cpp
virtual bool empty() const
```

---

#### addFile 

```cpp
virtual void addFile(const std::string & path)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### root 

```cpp
json::value & root
```

## Manifest 

> **Defined in:** `package.h`

This class provides a list of all package files and their location on the file system.

### Members

| Name | Description |
|------|-------------|
| [`Manifest`](#group__pacm_1ga27938a9ecf0a7d28a49a33fbf1cf24b8) |  |
| [`~Manifest`](#group__pacm_1ga9d7098ebc7d12fc1dbabd8c2d07aa05b) |  |
| [`empty`](#group__pacm_1gaa767187173b460a6ed903ad7b5237b87) |  |
| [`addFile`](#group__pacm_1ga46bf1459f6d73eb846287739d0b0c2f2) |  |
| [`root`](#group__pacm_1gaac9eb9eb02a577f627687939e65a3af7) |  |

---

#### Manifest 

```cpp
Manifest(json::value & src)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `json::value &` |  |

---

#### ~Manifest 

```cpp
virtual ~Manifest()
```

---

#### empty 

```cpp
virtual bool empty() const
```

---

#### addFile 

```cpp
virtual void addFile(const std::string & path)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### root 

```cpp
json::value & root
```

## PackagePair 

> **Defined in:** `package.h`

This class provides pairing of a local and a remote package.

### Members

| Name | Description |
|------|-------------|
| [`PackagePair`](#group__pacm_1ga0cfb58ccd49a5bff96f9c148ff7d39ef) |  |
| [`valid`](#group__pacm_1gaff5ec132e15164c53c6874e690fb4c6c) |  |
| [`id`](#group__pacm_1ga8bf565f8d76464a920733d01c68d9e78) |  |
| [`name`](#group__pacm_1ga19214f72c4a06d16587f9ca8d4fc5f9b) |  |
| [`type`](#group__pacm_1ga6ea1f67837c2ac9c11b46826b3eb79d6) |  |
| [`author`](#group__pacm_1ga53e2b423f63a656def7af02faa7ef00b) |  |
| [`local`](#group__pacm_1gadbaaae13eaaf203a08d8321bf0ca9bb0) | Returns true if there are no possible updates for this package, false otherwise. |
| [`remote`](#group__pacm_1ga64dacf8c8eafba27426e2f9a85cbd4b7) |  |

---

#### PackagePair 

```cpp
PackagePair(LocalPackage * local, RemotePackage * remote)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `local` | `LocalPackage *` |  |
| `remote` | `RemotePackage *` |  |

---

#### valid 

```cpp
virtual bool valid() const
```

---

#### id 

```cpp
std::string id() const
```

---

#### name 

```cpp
std::string name() const
```

---

#### type 

```cpp
std::string type() const
```

---

#### author 

```cpp
std::string author() const
```

---

#### local 

```cpp
LocalPackage * local
```

Returns true if there are no possible updates for this package, false otherwise.

---

#### remote 

```cpp
RemotePackage * remote
```

## Options 

> **Defined in:** `packagemanager.h`

[Package](#structscy_1_1pacm_1_1Package) manager initialization options.

### Members

| Name | Description |
|------|-------------|
| [`Options`](#group__pacm_1ga5bf57541a7fc15cae0283c80543060fb) |  |
| [`endpoint`](#group__pacm_1ga17eaf31ffc1fc663c74715c0861feaa7) | The HTTP server endpoint. |
| [`indexURI`](#group__pacm_1gaeaf07e705ed06b76a6e6da069b06933a) | The HTTP server URI for querying packages JSON. |
| [`httpUsername`](#group__pacm_1ga2d4089adeb8ea1b332289aa912f9670d) | Username for HTTP basic auth. |
| [`httpPassword`](#group__pacm_1gad72b21a0fd0afc7d0afbe79a267977c9) | PAssword for HTTP basic auth. |
| [`httpOAuthToken`](#group__pacm_1ga3ae63176c907fe93eafb52451c55aa5d) | Will be used instead of HTTP basic if provided. |
| [`tempDir`](#group__pacm_1ga9baae9668ea2afb09d479bc4e1622059) | Directory where package files will be downloaded and extracted. |
| [`dataDir`](#group__pacm_1ga2983a9e79dab4c044b56a53c3bec3ba0) | Directory where package manifests will be kept. |
| [`installDir`](#group__pacm_1ga18001156609c5fe9845f4d0c259964ff) | Directory where packages will be installed. |
| [`platform`](#group__pacm_1gaa9e36a87975600551306e8551065414b) | Platform (win32, linux, mac) |
| [`checksumAlgorithm`](#group__pacm_1gaa458d98644fdde252a9562c01028ad47) | Checksum algorithm (MDS/SHA1) |
| [`clearFailedCache`](#group__pacm_1ga791221a28415eff06ee9a162e950e9dd) | This flag tells the package manager weather or not to clear the package cache if installation fails. |

---

#### Options 

```cpp
inline Options(const std::string & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `const std::string &` |  |

---

#### endpoint 

```cpp
std::string endpoint
```

The HTTP server endpoint.

---

#### indexURI 

```cpp
std::string indexURI
```

The HTTP server URI for querying packages JSON.

---

#### httpUsername 

```cpp
std::string httpUsername
```

Username for HTTP basic auth.

---

#### httpPassword 

```cpp
std::string httpPassword
```

PAssword for HTTP basic auth.

---

#### httpOAuthToken 

```cpp
std::string httpOAuthToken
```

Will be used instead of HTTP basic if provided.

---

#### tempDir 

```cpp
std::string tempDir
```

Directory where package files will be downloaded and extracted.

---

#### dataDir 

```cpp
std::string dataDir
```

Directory where package manifests will be kept.

---

#### installDir 

```cpp
std::string installDir
```

Directory where packages will be installed.

---

#### platform 

```cpp
std::string platform
```

Platform (win32, linux, mac)

---

#### checksumAlgorithm 

```cpp
std::string checksumAlgorithm
```

Checksum algorithm (MDS/SHA1)

---

#### clearFailedCache 

```cpp
bool clearFailedCache
```

This flag tells the package manager weather or not to clear the package cache if installation fails.

