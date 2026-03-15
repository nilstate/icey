# util 

A simple message rate limiter based on the token bucket algorithm.

### Classes

| Name | Description |
|------|-------------|
| [`Base64PacketEncoder`](#classscy_1_1Base64PacketEncoder) |  |
| [`IDiagnostic`](#classscy_1_1IDiagnostic) |  |
| [`AsyncDiagnostic`](#classscy_1_1AsyncDiagnostic) |  |
| [`DiagnosticManager`](#classscy_1_1DiagnosticManager) |  |
| [`IRegistry`](#classscy_1_1IRegistry) |  |
| [`RateLimiter`](#classscy_1_1RateLimiter) |  |
| [`StreamManager`](#classscy_1_1StreamManager) |  |
| [`TimedManager`](#classscy_1_1TimedManager) | Timed pointer manager |
| [`BasicUser`](#classscy_1_1BasicUser) |  |
| [`UserManager`](#classscy_1_1UserManager) | > Deprecated: This class manages a list of users. |
| [`DiagnosticState`](#structscy_1_1DiagnosticState) |  |
| [`IUser`](#structscy_1_1IUser) |  |

## Base64PacketEncoder 

> **Extends:** `scy::PacketProcessor`
> **Defined in:** `base64packetencoder.h`

### Members

| Name | Description |
|------|-------------|
| [`Base64PacketEncoder`](#group__util_1ga7a4a4a092ea8e67ba50a511a57acaa0b) |  |
| [`process`](#group__util_1ga2180bb1a7753b37c2d12754eeb9d6e99) | This method performs processing on the given packet and emits the result. |
| [`emitter`](#group__util_1ga2d3eededaa9fe67484a81fb7a346dc03) |  |

---

#### Base64PacketEncoder 

```cpp
inline Base64PacketEncoder()
```

---

#### process 

```cpp
virtual inline void process(IPacket & packet)
```

This method performs processing on the given packet and emits the result.

Note: If packet processing is async (the packet is not in the current thread scope) then packet data must be copied. Copied data can be freed directly aFter the async call to [emit()](./doc/api-base.md#group__base_1ga995248310998c29df87051389f52b58c) the outgoing packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `packet` | `IPacket &` |  |

---

#### emitter 

```cpp
PacketSignal emitter
```

## IDiagnostic 

> **Extends:** `scy::Stateful< DiagnosticState >`
> **Subclasses:** `scy::AsyncDiagnostic`
> **Defined in:** `diagnosticmanager.h`

### Members

| Name | Description |
|------|-------------|
| [`IDiagnostic`](#group__util_1gac8aca5149fda66a927334e1132c48890) |  |
| [`~IDiagnostic`](#group__util_1ga4ef2121b9fd45a42fda1b2aee0a9ea2c) |  |
| [`check`](#group__util_1gabd4bf909e78503107e0604f11d0437f6) |  |
| [`reset`](#group__util_1ga4824c0cc08d8b244a5b7a8916e50a136) |  |
| [`complete`](#group__util_1ga23369ac4b799996b3b2911e05076e557) |  |
| [`passed`](#group__util_1ga5ed635d12653d66026810386a4bfb7a1) |  |
| [`failed`](#group__util_1ga31ceeef28f1b085ec2162002deb00d60) |  |
| [`run`](#group__util_1ga36bfaa138a649addc6f0d89d4df5ea28) | Override to implement diagnostic logic. |
| [`pass`](#group__util_1ga673e4cb65e024a8ec9e123061689b112) |  |
| [`fail`](#group__util_1ga05af62ad19e7683f04b2a0637ae0baed) |  |
| [`addSummary`](#group__util_1ga64ac18ee28d24cec3f898123db0a62c8) |  |
| [`name`](#group__util_1gaa69e4ded0fb7d0db9113ab8c318a043f) | The name of the diagnostic. |
| [`description`](#group__util_1ga5a7dca63479b6fb69f62ed3c33919a11) | The diagnostic description. |
| [`summary`](#group__util_1gac24ce2f6e0d721dc04d292d6b99e2beb) | The diagnostic summary, maybe including troubleshooting information on failure. |
| [`SummaryUpdated`](#group__util_1ga0383aed91dd872f4f1d6326a882e8922) | Signals when a new text item is added to the summary. |

---

#### IDiagnostic 

```cpp
IDiagnostic()
```

---

#### ~IDiagnostic 

```cpp
virtual ~IDiagnostic()
```

---

#### check 

```cpp
virtual void check()
```

---

#### reset 

```cpp
virtual void reset()
```

---

#### complete 

```cpp
virtual bool complete() const
```

---

#### passed 

```cpp
virtual bool passed() const
```

---

#### failed 

```cpp
virtual bool failed() const
```

---

#### run 

```cpp
void run()
```

Override to implement diagnostic logic.

The StateChange signal will dispatch diagnostic test results to delegates.

---

#### pass 

```cpp
virtual bool pass()
```

---

#### fail 

```cpp
virtual bool fail()
```

---

#### addSummary 

```cpp
virtual void addSummary(const std::string & text)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `const std::string &` |  |

---

#### name 

```cpp
std::string name
```

The name of the diagnostic.

---

#### description 

```cpp
std::string description
```

The diagnostic description.

---

#### summary 

```cpp
std::vector< std::string > summary
```

The diagnostic summary, maybe including troubleshooting information on failure.

---

#### SummaryUpdated 

```cpp
Signal< void(const std::string &)> SummaryUpdated
```

Signals when a new text item is added to the summary.

## AsyncDiagnostic 

> **Extends:** `scy::IDiagnostic`, `scy::basic::Runnable`
> **Defined in:** `diagnosticmanager.h`

### Members

| Name | Description |
|------|-------------|
| [`~AsyncDiagnostic`](#group__util_1ga505c905b8a86b311f10b1bc81f3dd3e9) |  |
| [`run`](#group__util_1ga620c69a7b25a89564232ab870c57c956) | Override to implement diagnostic logic. |
| [`check`](#group__util_1gab2c4393ef71dc2d6d090ed5eabe01224) |  |
| [`_thread`](#group__util_1ga86a906cd8701f49e2f2065c9e8e3c1f3) |  |

---

#### ~AsyncDiagnostic 

```cpp
virtual inline ~AsyncDiagnostic()
```

---

#### run 

```cpp
void run()
```

Override to implement diagnostic logic.

The StateChange signal will dispatch diagnostic test results to delegates.

---

#### check 

```cpp
virtual inline void check()
```

---

#### _thread 

```cpp
Thread _thread
```

## DiagnosticManager 

> **Extends:** `scy::PointerCollection< TKey, TValue >`
> **Defined in:** `diagnosticmanager.h`

### Members

| Name | Description |
|------|-------------|
| [`DiagnosticManager`](#group__util_1ga1f7e1a71860acf1acf2ad00e658168b4) |  |
| [`~DiagnosticManager`](#group__util_1ga33969db867a5da02ba66af577888cce1) |  |
| [`freeDiagnostic`](#group__util_1gafcfa5828dcb06aa0da5d926026fba952) |  |
| [`addDiagnostic`](#group__util_1ga965c646fe610d64eee38396d477e1527) | Adds a [IDiagnostic](#classscy_1_1IDiagnostic) test instance. |
| [`getDiagnostic`](#group__util_1gac6ba64b207ffd86739f287a1efda9e73) | Returns the [IDiagnostic](#classscy_1_1IDiagnostic) instance or throws a NotFoundException exception. |
| [`resetAll`](#group__util_1gaf46ebc8e54d7508f44dc5ad49c0e269f) |  |
| [`checkAll`](#group__util_1ga32981bf8dbec2deef3f70427d48c2e54) | Runs all managed [IDiagnostic](#classscy_1_1IDiagnostic) tests. DiagnosticsComplete will be dispatched on completion. |
| [`allComplete`](#group__util_1gad077e09ee488e3639b0188c8667f2477) |  |
| [`onDiagnosticStateChange`](#group__util_1gae5adc21f9a476721730d164a13c8dcb3) |  |
| [`DiagnosticsComplete`](#group__util_1gae4bffe1d36c1087a6456bdc124740968) |  |

---

#### DiagnosticManager 

```cpp
DiagnosticManager()
```

---

#### ~DiagnosticManager 

```cpp
virtual ~DiagnosticManager()
```

---

#### freeDiagnostic 

```cpp
bool freeDiagnostic(const std::string & name)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### addDiagnostic 

```cpp
bool addDiagnostic(IDiagnostic * test)
```

Adds a [IDiagnostic](#classscy_1_1IDiagnostic) test instance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `test` | `IDiagnostic *` |  |

---

#### getDiagnostic 

```cpp
virtual IDiagnostic * getDiagnostic(const std::string & name)
```

Returns the [IDiagnostic](#classscy_1_1IDiagnostic) instance or throws a NotFoundException exception.

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |

---

#### resetAll 

```cpp
virtual void resetAll()
```

---

#### checkAll 

```cpp
virtual void checkAll()
```

Runs all managed [IDiagnostic](#classscy_1_1IDiagnostic) tests. DiagnosticsComplete will be dispatched on completion.

---

#### allComplete 

```cpp
virtual bool allComplete()
```

---

#### onDiagnosticStateChange 

```cpp
virtual void onDiagnosticStateChange(void *, DiagnosticState & state, const DiagnosticState &)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `state` | `DiagnosticState &` |  |

---

#### DiagnosticsComplete 

```cpp
NullSignal DiagnosticsComplete
```

## IRegistry 

> **Defined in:** `iregistry.h`

### Members

| Name | Description |
|------|-------------|
| [`createT`](#group__util_1gaea23db6c38a420f33f2a0f7811bb02f1) |  |
| [`IRegistry`](#group__util_1ga1c7634f07f5a50280145164b4948ec71) |  |
| [`~IRegistry`](#group__util_1gae6ab7fa3040b6ccb7539a4e4a989cc6d) |  |
| [`createInstance`](#group__util_1gaea046c8664de5a458c5d8eb30f03c976) |  |
| [`registerType`](#group__util_1gae25f5528b11ce12d628e246eeb7e0e78) |  |
| [`unregisterType`](#group__util_1ga6501164e448376e278823594a9af0d93) |  |
| [`types`](#group__util_1ga2b6c7be0bba65bcbe8131921459f0d3f) |  |
| [`TypeRegistered`](#group__util_1gafea9585c6ff2924b3217fa26cb1a6c83) |  |
| [`TypeUnregistered`](#group__util_1gab4b972dc17709d48a418073e33e264b8) |  |
| [`_types`](#group__util_1ga6caeb590a40377bee11802fcd7153962) |  |

---

#### createT 

```cpp
template<typename T> inline ItemT * createT()
```

---

#### IRegistry 

```cpp
IRegistry() = default
```

---

#### ~IRegistry 

```cpp
virtual ~IRegistry() = default
```

---

#### createInstance 

```cpp
virtual inline ItemT * createInstance(const std::string & s)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `const std::string &` |  |

---

#### registerType 

```cpp
template<typename T> inline void registerType(const std::string & s)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `const std::string &` |  |

---

#### unregisterType 

```cpp
virtual inline void unregisterType(const std::string & s)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `s` | `const std::string &` |  |

---

#### types 

```cpp
inline TypeMap types() const
```

---

#### TypeRegistered 

```cpp
Signal< void(const std::string &)> TypeRegistered
```

---

#### TypeUnregistered 

```cpp
Signal< void(const std::string &)> TypeUnregistered
```

---

#### _types 

```cpp
TypeMap _types
```

## RateLimiter 

> **Defined in:** `ratelimiter.h`

### Members

| Name | Description |
|------|-------------|
| [`RateLimiter`](#group__util_1gaba3e054e11999e0faf0d8be12e31855e) |  |
| [`canSend`](#group__util_1gaa714f604007b21bc6435d3e518d68de4) |  |
| [`rate`](#group__util_1ga91c35e10dfcf207f17e62d2242708f26) | How many messages. |
| [`seconds`](#group__util_1gac47e0bd0461efd33ba6d7e480eeb9059) | Over how many seconds. |
| [`allowance`](#group__util_1ga40eb12dada557ee2fff2ad2e419ec92c) | Remaining send allowance. |
| [`lastCheck`](#group__util_1ga247fefbc3361b60286b4e431410ff98a) | Last time [canSend()](#group__util_1gaa714f604007b21bc6435d3e518d68de4) was called. |

---

#### RateLimiter 

```cpp
inline RateLimiter(double rate, double seconds)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `rate` | `double` |  |
| `seconds` | `double` |  |

---

#### canSend 

```cpp
inline bool canSend()
```

---

#### rate 

```cpp
double rate
```

How many messages.

---

#### seconds 

```cpp
double seconds
```

Over how many seconds.

---

#### allowance 

```cpp
double allowance
```

Remaining send allowance.

---

#### lastCheck 

```cpp
clock_t lastCheck
```

Last time [canSend()](#group__util_1gaa714f604007b21bc6435d3e518d68de4) was called.

## StreamManager 

> **Extends:** `scy::LiveCollection< TKey, TValue >`
> **Defined in:** `streammanager.h`

### Members

| Name | Description |
|------|-------------|
| [`StreamManager`](#group__util_1ga1f895ea996be0a12e49f0e44e4427ce2) |  |
| [`~StreamManager`](#group__util_1ga0204c0879b0c83ce9cd770ff14327669) |  |
| [`addStream`](#group__util_1gaf630cdcb67fa136f37e4b8ae9f07cf05) |  |
| [`closeStream`](#group__util_1ga4d9fe6b1424912270ee3669c093d6ac3) |  |
| [`closeAll`](#group__util_1ga6a82d6d92fa742e14402400791ef12a0) |  |
| [`getStream`](#group__util_1ga07cf385a3eb2fa5e1e4a4d236dbc6b3c) |  |
| [`getDafaultStream`](#group__util_1ga7cc76a987b8398d5c12cb01575bad6f0) | Returns the first stream in the list, or NULL. |
| [`streams`](#group__util_1ga65bc8e98624a30595536f5feca2024ad) |  |
| [`print`](#group__util_1gaf885fbf7d31d61dd1545862913c34e94) |  |
| [`onAdd`](#group__util_1ga1353a4e0b6b655a9e433b2dcf727d5b6) | Called after a stream is added. |
| [`onRemove`](#group__util_1gaef450cb0b58f02c844a8d31657f67822) | Called after a stream is removed. |
| [`onStreamStateChange`](#group__util_1ga7388ae78948b7e42d5ef65470d97022c) |  |
| [`className`](#group__util_1ga0d838f9621cbf8455f08d6cdc259fc23) |  |

---

#### StreamManager 

```cpp
StreamManager()
```

---

#### ~StreamManager 

```cpp
virtual ~StreamManager()
```

---

#### addStream 

```cpp
virtual bool addStream(PacketStream * stream, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | `PacketStream *` |  |
| `whiny` | `bool` |  |

---

#### closeStream 

```cpp
virtual bool closeStream(const std::string & name, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### closeAll 

```cpp
virtual void closeAll()
```

---

#### getStream 

```cpp
virtual PacketStream * getStream(const std::string & name, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### getDafaultStream 

```cpp
virtual PacketStream * getDafaultStream()
```

Returns the first stream in the list, or NULL.

---

#### streams 

```cpp
virtual const Map & streams() const
```

---

#### print 

```cpp
virtual void print(std::ostream & os) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

#### onAdd 

```cpp
virtual void onAdd(const std::string &, PacketStream * task)
```

Called after a stream is added.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `PacketStream *` |  |

---

#### onRemove 

```cpp
virtual void onRemove(const std::string &, PacketStream * task)
```

Called after a stream is removed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `PacketStream *` |  |

---

#### onStreamStateChange 

```cpp
virtual void onStreamStateChange(void * sender, PacketStreamState & state, const PacketStreamState &)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sender` | `void *` |  |
| `state` | `PacketStreamState &` |  |

---

#### className 

```cpp
virtual inline const char * className() const
```

## TimedManager 

> **Extends:** `scy::PointerCollection< TKey, TValue >`
> **Defined in:** `timedmanager.h`

Timed pointer manager

Provides timed persistent data storage for class instances. TValue must implement the clone() method.

### Members

| Name | Description |
|------|-------------|
| [`TimedManager`](#group__util_1gabe8bdf4cce5c58328201adb38ac8189d) |  |
| [`~TimedManager`](#group__util_1ga26d523897eb3a1df55e1d08e145adc8b) |  |
| [`add`](#group__util_1gaf24ec59ddd41c6995cbbc4c36694d8d5) | Add an item which will expire (and be deleted) after the specified timeout value. If the timeout is 0 the item will be stored indefinitely. The [TimedManager](#classscy_1_1TimedManager) assumes ownership of the given pointer. |
| [`expires`](#group__util_1gaab30aca59aa37b7d54435095eadfebbf) | Update the item expiry timeout. |
| [`expires`](#group__util_1ga82593ffb8f8a37f8803b1472cffa0460) | Update the item expiry timeout. |
| [`clear`](#group__util_1ga182cfabe68dcb766dca8a3dc1934f128) |  |
| [`setTimeout`](#group__util_1gac948ff0dba56d8682862800b5e9cc90b) |  |
| [`onRemove`](#group__util_1ga16b8d3dd608e5034e2f90e776fd096ae) |  |
| [`onTimeout`](#group__util_1ga6344f6f8576af612ce4a4d73c8e35cc4) |  |
| [`onTimerUpdate`](#group__util_1ga234ef400c37fa8b8df13454853f124c3) |  |
| [`_tmutex`](#group__util_1ga02e84f8577dfe8953dd646f6ba13eb63) |  |
| [`_timeouts`](#group__util_1ga92175954595f2f17eda515d21ce23283) |  |
| [`_timer`](#group__util_1gadf29428f84cad7b8fe48af01c9899fb7) |  |

---

#### TimedManager 

```cpp
inline TimedManager(uv::Loop * loop)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `loop` | `uv::Loop *` |  |

---

#### ~TimedManager 

```cpp
virtual inline ~TimedManager()
```

---

#### add 

```cpp
virtual inline void add(const TKey & key, TValue * item, long timeout)
```

Add an item which will expire (and be deleted) after the specified timeout value. If the timeout is 0 the item will be stored indefinitely. The [TimedManager](#classscy_1_1TimedManager) assumes ownership of the given pointer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `item` | `TValue *` |  |
| `timeout` | `long` |  |

---

#### expires 

```cpp
virtual inline bool expires(const TKey & key, long timeout)
```

Update the item expiry timeout.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `timeout` | `long` |  |

---

#### expires 

```cpp
virtual inline bool expires(TValue * item, long timeout)
```

Update the item expiry timeout.

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `TValue *` |  |
| `timeout` | `long` |  |

---

#### clear 

```cpp
virtual inline void clear()
```

---

#### setTimeout 

```cpp
virtual inline bool setTimeout(TValue * item, long timeout)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `TValue *` |  |
| `timeout` | `long` |  |

---

#### onRemove 

```cpp
virtual inline void onRemove(const TKey & key, TValue * item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const TKey &` |  |
| `item` | `TValue *` |  |

---

#### onTimeout 

```cpp
virtual inline void onTimeout(TValue * item)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `item` | `TValue *` |  |

---

#### onTimerUpdate 

```cpp
inline void onTimerUpdate()
```

---

#### _tmutex 

```cpp
std::mutex _tmutex
```

---

#### _timeouts 

```cpp
TimeoutMap _timeouts
```

---

#### _timer 

```cpp
Timer _timer
```

## BasicUser 

> **Extends:** `scy::IUser`
> **Defined in:** `usermanager.h`

### Members

| Name | Description |
|------|-------------|
| [`BasicUser`](#group__util_1ga99d6a8ec8d148263b858607fc12e6915) |  |
| [`username`](#group__util_1gaa4d74cffed88430c95be50f4178a407c) |  |
| [`password`](#group__util_1gaf85d948cee382cb7a8fd63bce2230906) |  |
| [`_username`](#group__util_1ga984dc62d5f29dad4f46adb0a18709614) |  |
| [`_password`](#group__util_1ga6049b540334bbb0faa236a6a78646fb0) |  |

---

#### BasicUser 

```cpp
inline BasicUser(const std::string & username, const std::string & password)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `username` | `const std::string &` |  |
| `password` | `const std::string &` |  |

---

#### username 

```cpp
virtual inline std::string username() const
```

---

#### password 

```cpp
virtual inline std::string password() const
```

---

#### _username 

```cpp
std::string _username
```

---

#### _password 

```cpp
std::string _password
```

## UserManager 

> **Extends:** `scy::LiveCollection< std::string, IUser >`
> **Defined in:** `usermanager.h`

> Deprecated: This class manages a list of users.

### Members

| Name | Description |
|------|-------------|
| [`UserManager`](#group__util_1gacc943bf23bcb3a3d2ce174dc52da725b) |  |
| [`~UserManager`](#group__util_1ga8551f4207a9d4898cf43366d10d5e2d0) |  |
| [`add`](#group__util_1ga7885b358f8dad3a9c1bfa26351919eb3) |  |

---

#### UserManager 

```cpp
UserManager() = default
```

---

#### ~UserManager 

```cpp
virtual ~UserManager() = default
```

---

#### add 

```cpp
virtual inline bool add(IUser * user)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `user` | `IUser *` |  |

## DiagnosticState 

> **Extends:** `scy::State`
> **Defined in:** `diagnosticmanager.h`

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__util_1ga3d95bbbd220fafe8e34708b6edebe92a) |  |
| [`str`](#group__util_1ga0ff5f446bcd0e549324821cbd482ff04) |  |

---

#### Type 

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `None` |  |
| `Checking` |  |
| `Passed` |  |
| `Failed` |  |

---

#### str 

```cpp
inline std::string str(unsigned int id) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `unsigned int` |  |

## IUser 

> **Subclasses:** `scy::BasicUser`
> **Defined in:** `usermanager.h`

### Members

| Name | Description |
|------|-------------|
| [`username`](#group__util_1gab1897a1d6cd4507762cb93b1a8144c3c) |  |
| [`password`](#group__util_1gad90f6bab88fd34f0a1644c2ec43b5e5f) |  |

---

#### username 

```cpp
std::string username() const
```

---

#### password 

```cpp
std::string password() const
```

