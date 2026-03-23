# STUN

> RFC 5389 message codec, typed attributes, HMAC-SHA1 integrity, and transaction layer for NAT traversal.

**[API Reference →](../api/stun.md)** · **[Source →](../../src/stun/)**

## Overview

STUN (Session Traversal Utilities for NAT, RFC 5389) is the wire protocol underpinning all NAT traversal in Icey. Every TURN message is a STUN message with a TURN-specific method. Every ICE connectivity check is a STUN Binding request. The `stun` module provides the complete codec for both, and the TURN module is built entirely on top of it.

The module covers four concerns:

- **Wire format**: 20-byte fixed header, 4-byte attribute headers, 4-byte alignment padding, magic cookie validation.
- **Typed attribute hierarchy**: every attribute defined in RFC 5389, RFC 5766, and RFC 6062 has a concrete C++ class with a static `TypeID` that drives the template accessor on `Message`.
- **Message integrity**: HMAC-SHA1 computation on write and verification on read, using the long-term credential key `MD5(username:realm:password)`.
- **Transaction layer**: request/response matching by transaction ID, with configurable timeout and retry count.

Link against `Icey::stun`:

```cmake
target_link_libraries(myapp PRIVATE Icey::stun)
```

Headers:

```cpp
#include "icy/stun/stun.h"        // wire constants, AddressFamily
#include "icy/stun/message.h"     // Message, TransactionID
#include "icy/stun/attributes.h"  // full attribute hierarchy
#include "icy/stun/transaction.h" // Transaction
```

## Architecture

```text
stun::Message
├── ClassType   (Request / Indication / SuccessResponse / ErrorResponse)
├── MethodType  (Binding, Allocate, Refresh, SendIndication, ...)
├── TransactionID  (12-byte opaque string, randomly generated)
└── std::vector<std::unique_ptr<Attribute>>
        ├── AddressAttribute    → XorMappedAddress, XorPeerAddress, XorRelayedAddress, ...
        ├── UInt8Attribute      → EventPort
        ├── UInt32Attribute     → Lifetime, RequestedTransport, ChannelNumber, ...
        ├── UInt64Attribute     → ICEControlling, ICEControlled
        ├── FlagAttribute       → ICEUseCandidate, DontFragment
        ├── StringAttribute     → Username, Realm, Nonce, Data, Software, ...
        ├── UInt16ListAttribute → UnknownAttributes
        ├── MessageIntegrity    (HMAC-SHA1, 20 bytes)
        └── ErrorCode           (class + number + reason phrase)

stun::Transaction  (extends net::Transaction<Message>)
├── sends Request over a net::Socket
├── matches Response by transaction ID
└── invokes state-machine callbacks on success, error, or timeout
```

`Message` extends `IPacket` and is copyable, movable, and cloneable. Attribute ownership is held in a `vector<unique_ptr<Attribute>>`; `get<T>()` returns a raw (non-owning) pointer for read/write access.

## Usage

### Wire format constants

`stun.h` exposes the RFC 5389 layout constants directly:

```cpp
#include "icy/stun/stun.h"

constexpr int      kMessageHeaderSize   = 20;   // fixed header
constexpr int      kAttributeHeaderSize = 4;    // type (2) + length (2)
constexpr int      kTransactionIdOffset = 8;    // offset of txn ID in header
constexpr int      kTransactionIdLength = 12;   // 96-bit transaction ID
constexpr uint32_t kMagicCookie        = 0x2112A442; // bytes 4-7
```

The magic cookie occupies bytes 4-7 of every RFC 5389 message, distinguishing it from legacy RFC 3489 packets. The parser rejects messages that do not carry it.

`AddressFamily` maps wire address-family bytes to a typed enum:

```cpp
enum class AddressFamily : uint8_t { Undefined = 0, IPv4 = 1, IPv6 = 2 };
```

### Constructing messages

`Message` is the central class. Construct it with an explicit class and method:

```cpp
#include "icy/stun/message.h"

// A STUN Binding request with a randomly generated transaction ID
stun::Message req(stun::Message::Request, stun::Message::Binding);

// A TURN Allocate request
stun::Message alloc(stun::Message::Request, stun::Message::Allocate);

// Copy and move are both deep (clones all attributes)
stun::Message copy = req;
stun::Message moved = std::move(alloc);
```

The default constructor (`Message()`) produces a `Request / Undefined` message; the two-argument form is what you normally want.

### Class and method types

The four class values map directly to RFC 5389 wire bits:

| Enumerator | Wire bits | Usage |
| --- | --- | --- |
| `Request` | `0x0000` | Initiates a transaction; expects a response |
| `Indication` | `0x0010` | Fire-and-forget; no response expected |
| `SuccessResponse` | `0x0100` | Positive reply to a Request |
| `ErrorResponse` | `0x0110` | Error reply to a Request |

The method type covers both STUN and TURN, since they share the same message envelope:

| Method | Value | RFC | Notes |
| --- | --- | --- | --- |
| `Binding` | `0x0001` | 5389 | NAT discovery |
| `Allocate` | `0x0003` | 5766 | Request relay allocation |
| `Refresh` | `0x0004` | 5766 | Extend or delete allocation |
| `SendIndication` | `0x0006` | 5766 | Client-to-peer data relay; indication only |
| `DataIndication` | `0x0007` | 5766 | Server-to-client data delivery; indication only |
| `CreatePermission` | `0x0008` | 5766 | Install peer permission |
| `ChannelBind` | `0x0009` | 5766 | Bind channel number to peer address |
| `Connect` | `0x000a` | 6062 | Request TCP connection to peer |
| `ConnectionBind` | `0x000b` | 6062 | Associate data socket with connection ID |
| `ConnectionAttempt` | `0x000c` | 6062 | Server notification of inbound peer TCP connection |

`isValidMethod(uint16_t)` is a `constexpr` function used by the parser to reject unknown method bytes before constructing any attribute objects.

### Transaction ID

The transaction ID is a 12-byte opaque value stored as `std::string` (binary, not printable). The two-argument constructor generates a cryptographically random ID automatically:

```cpp
stun::Message req(stun::Message::Request, stun::Message::Allocate);
const stun::TransactionID& tid = req.transactionID(); // 12 bytes

// Override with an explicit ID (rarely needed)
req.setTransactionID(someExistingID);
```

Random generation is correct for almost every use case. The only time you need `setTransactionID()` is when replicating a message across transports with a pre-assigned ID.

### Adding attributes

`Message::add()` accepts either a raw pointer (taking ownership) or a `unique_ptr`:

```cpp
#include "icy/stun/attributes.h"

// Ownership via raw pointer
auto* user = new stun::Username();
user->copyBytes("alice");
req.add(user); // message takes ownership

// Ownership via unique_ptr
req.add(std::make_unique<stun::Lifetime>());
```

Attributes are stored in insertion order. The wire format preserves this order. `MESSAGE-INTEGRITY` must be the last attribute before `FINGERPRINT`; add it after all other attributes.

### Retrieving attributes

The typed template accessor uses the concrete class's `TypeID` to avoid any casting at the call site:

```cpp
// Type-safe: returns stun::Lifetime* or nullptr
auto* lifetime = req.get<stun::Lifetime>();
if (lifetime)
    lifetime->setValue(600); // seconds

// Multiple occurrences: zero-based index
auto* peer0 = req.get<stun::XorPeerAddress>(0);
auto* peer1 = req.get<stun::XorPeerAddress>(1);

// By raw type code (returns Attribute*, requires cast)
auto* attr = req.get(stun::Attribute::Lifetime);
```

### Serialisation and parsing

```cpp
#include "icy/stun/message.h"

// Write to wire format
Buffer buf;
req.write(buf);

// Parse from wire format
stun::Message parsed;
ssize_t consumed = parsed.read(ConstBuffer(buf));
if (consumed == 0) {
    // Parse failed: malformed packet, unrecognised method, or incomplete data
}
```

`read()` returns the number of bytes consumed, not a boolean. On TCP a single receive buffer can contain multiple back-to-back STUN messages; read the return value, advance your buffer pointer, and call `read()` again. On UDP each datagram is a single message.

Diagnostic output:

```cpp
std::cout << req.toString();     // "ALLOCATE/Request [txn=...] attrs=[USERNAME, LIFETIME, ...]"
req.print(std::cout);            // same
std::cout << req.methodString(); // "ALLOCATE"
std::cout << req.classString();  // "Request"
```

### Error codes

`Message::ErrorCodes` lists every STUN and TURN error code:

| Code | Enumerator | Meaning |
| --- | --- | --- |
| 300 | `TryAlternate` | Redirect to alternate server |
| 400 | `BadRequest` | Malformed request |
| 401 | `NotAuthorized` | Missing or bad credentials |
| 403 | `Forbidden` | Not allowed |
| 420 | `UnknownAttribute` | Unrecognised comprehension-required attribute |
| 437 | `AllocationMismatch` | 5-tuple already has a different allocation |
| 438 | `StaleNonce` | Nonce has expired; retry with new nonce |
| 441 | `WrongCredentials` | Credentials don't match the allocation |
| 442 | `UnsupportedTransport` | Transport not supported |
| 446 | `ConnectionAlreadyExists` | RFC 6062: duplicate connection attempt |
| 447 | `ConnectionTimeoutOrFailure` | RFC 6062: peer TCP connection failed |
| 486 | `AllocationQuotaReached` | Per-user quota exceeded |
| 487 | `RoleConflict` | ICE role conflict |
| 500 | `ServerError` | Internal server error |
| 508 | `InsufficientCapacity` | Server out of resources |

`errorString(code)` maps any of these to a human-readable string for log output.

### Attributes

#### Hierarchy

All attributes inherit from `Attribute`, which provides the wire type code, body size, padding helpers, and the `create()` factory. Concrete types are instantiated by `Attribute::create()` during `Message::read()` based on the wire type code:

```text
Attribute  (abstract)
├── AddressAttribute   (8 or 20 bytes; handles XOR encoding)
│   ├── MappedAddress      0x0001
│   ├── XorMappedAddress   0x0020  ← standard STUN reflex
│   ├── XorPeerAddress     0x0012  ← TURN peer
│   ├── XorRelayedAddress  0x0016  ← TURN relay
│   ├── AlternateServer    0x000e
│   └── SourceAddress, DestinationAddress, ...
├── UInt8Attribute   (1 byte)
│   └── EventPort          0x0018
├── UInt32Attribute  (4 bytes; bit-level accessors)
│   ├── Fingerprint        0x8028
│   ├── RequestedTransport 0x0019  ← upper byte = IANA protocol
│   ├── Lifetime           0x000d
│   ├── ChannelNumber      0x000c
│   ├── ICEPriority        0x0024
│   └── ConnectionID       0x002a  ← RFC 6062
├── UInt64Attribute  (8 bytes; bit-level accessors)
│   ├── ICEControlling     0x802A
│   └── ICEControlled      0x8029
├── FlagAttribute    (0 bytes; presence is the value)
│   ├── ICEUseCandidate    0x0025
│   └── DontFragment       0x001A
├── StringAttribute  (variable bytes)
│   ├── Username           0x0006
│   ├── Realm              0x0014
│   ├── Nonce              0x0015
│   ├── Software           0x8022
│   ├── Data               0x0013  ← TURN payload carrier
│   └── ReservationToken   0x0022  (8 bytes)
├── UInt16ListAttribute (variable; list of type codes)
│   └── UnknownAttributes  0x000a
├── MessageIntegrity   0x0008  (20-byte HMAC-SHA1)
└── ErrorCode          0x0009  (class + number + reason)
```

Concrete types are declared with two macros that inject a static `TypeID` constant, enabling the template accessor without any registration table:

```cpp
// Fixed-size: body size is always the same
DECLARE_FIXLEN_STUN_ATTRIBUTE(Lifetime, 0x000d, UInt32Attribute)

// Variable-size: takes an initial size; 0 means empty until set
DECLARE_STUN_ATTRIBUTE(Username, 0x0006, StringAttribute, 0)
```

#### Address attributes and XOR encoding

RFC 5389 section 15.2 requires `XOR-MAPPED-ADDRESS`, `XOR-PEER-ADDRESS`, and `XOR-RELAYED-ADDRESS` to XOR the IPv4 address against the magic cookie and the port against the high 16 bits of the magic cookie. This prevents NAT devices from rewriting embedded addresses. `AddressAttribute::read()` and `write()` handle the XOR transformation transparently:

```cpp
// Reading a reflexive address from a Binding response
auto* xma = response.get<stun::XorMappedAddress>();
if (xma) {
    net::Address reflexive = xma->address(); // already XOR-decoded
    // use reflexive.ip() and reflexive.port()
}

// Writing a peer address into a CreatePermission request
auto* peer = new stun::XorPeerAddress();
peer->setAddress(net::Address("203.0.113.42", 5000)); // stored plain; XORed on write
req.add(peer);
```

`AddressAttribute` stores both IPv4 and IPv6; `family()` returns the STUN `AddressFamily` enum. Wire sizes are `AddressAttribute::IPv4Size = 8` and `AddressAttribute::IPv6Size = 20`.

#### Integer attributes

`UInt32Attribute` and `UInt64Attribute` provide `value()` / `setValue()` and bit-level accessors:

```cpp
// Lifetime: a plain 32-bit second count
auto* lt = new stun::Lifetime();
lt->setValue(600); // 600 seconds

// RequestedTransport: upper byte = IANA protocol number
// 17 = UDP, 6 = TCP
auto* rt = new stun::RequestedTransport();
rt->setValue(kProtocolUDP << 24); // set to UDP

// ICEControlling: 64-bit tie-breaker
auto* ctrl = new stun::ICEControlling();
ctrl->setValue(tieBreaker); // uint64_t

// Bit-level access
bool bit = rt->getBit(29);
rt->setBit(29, true);
```

`UInt16ListAttribute` (used only by `UnknownAttributes`) stores a variable-length list of type codes:

```cpp
auto* ua = new stun::UnknownAttributes();
ua->addType(stun::Attribute::Lifetime);
ua->addType(stun::Attribute::RequestedTransport);
// ua->size() == 2
```

#### String attributes

`StringAttribute` holds an arbitrary byte buffer and exposes both raw and string interfaces:

```cpp
auto* user = new stun::Username();
user->copyBytes("alice");            // from null-terminated string
user->copyBytes(ptr, length);        // from arbitrary bytes
std::string s = user->asString();    // back to std::string

// Single-byte access
uint8_t b = user->getByte(0);
user->setByte(0, 'A');

// Raw bytes (not null-terminated)
const char* raw = user->bytes();
```

`Data` (type `0x0013`) is the payload carrier for Send and Data Indications. Use `copyBytes()` to set the payload:

```cpp
auto* data = new stun::Data();
data->copyBytes(payload, payloadSize);
indication.add(data);
```

#### Flag attributes

`FlagAttribute` has a body size of exactly zero. Presence in the message means the flag is set; absence means it is not. Reading and writing are both no-ops on the body:

```cpp
// ICE use-candidate flag
req.add(new stun::ICEUseCandidate()); // presence is the signal

// Check for presence
if (req.get<stun::ICEUseCandidate>()) {
    // use-candidate is set
}
```

### Message Integrity

`MessageIntegrity` (type `0x0008`) implements HMAC-SHA1 over the message bytes up to but not including the integrity attribute itself, with the message length field pre-adjusted to include the attribute. This is the exact computation specified in RFC 5389 section 15.4.

#### Long-term credential key derivation

The key is always `MD5(username ":" realm ":" password)`:

```cpp
#include "icy/crypto/hash.h"

std::string deriveKey(const std::string& username,
                      const std::string& realm,
                      const std::string& password)
{
    crypto::Hash md5("md5");
    md5.update(username + ":" + realm + ":" + password);
    return md5.digestStr(); // 16-byte binary MD5 digest
}
```

The `crypto` module provides `crypto::Hash`. See [crypto.md](crypto.md) for the full hash API.

#### Signing outgoing messages

Set the key on the `MessageIntegrity` attribute before calling `write()`. The HMAC is computed lazily during serialisation:

```cpp
std::string key = deriveKey("alice", "example.com", "s3cr3t");

auto* integrity = new stun::MessageIntegrity();
integrity->setKey(key);
req.add(integrity); // add last (before FINGERPRINT, if used)

Buffer buf;
req.write(buf); // HMAC computed here over all preceding attribute bytes
```

#### Verifying incoming messages

`read()` captures the raw message bytes seen so far and the 20-byte HMAC from the wire into the attribute. Call `verifyHmac()` after parsing:

```cpp
stun::Message msg;
msg.read(incomingBuffer);

auto* integrity = msg.get<stun::MessageIntegrity>();
if (!integrity) {
    // reject: no MESSAGE-INTEGRITY present
}
if (!integrity->verifyHmac(key)) {
    // reject: HMAC mismatch; packet tampered or wrong credentials
}
```

The TURN server sample uses this pattern in `authenticateRequest()`; see [turn.md](turn.md) for the full authentication flow.

#### Error code attribute

`ErrorCode` encodes STUN errors as a class byte (hundreds digit, 3-6), a number byte (0-99), and an optional UTF-8 reason phrase:

```cpp
auto* err = new stun::ErrorCode();
err->setErrorCode(438);          // StaleNonce
err->setReason("Stale Nonce");
response.add(err);

// Reconstruct the integer: class * 100 + number
int code = err->errorCode(); // 438
```

On read, both `errorClass()` (returns 4 for 4xx) and `errorNumber()` (returns the trailing digits) are available independently if needed for fine-grained branching.

### Transactions

`stun::Transaction` wraps a single request/response exchange. It extends `net::Transaction<Message>` with STUN-specific transaction ID matching and response class inference.

#### Construction and use

```cpp
#include "icy/stun/transaction.h"

auto socket = std::make_shared<net::UDPSocket>();
net::Address server("203.0.113.1", 3478);

auto* tx = new stun::Transaction(socket, server,
    /*timeout_ms=*/10000,
    /*retries=*/1);

tx->request() = stun::Message(stun::Message::Request, stun::Message::Binding);
tx->send();
```

The base class `net::Transaction<Message>` manages send/retry/timeout. `stun::Transaction` overrides two methods:

- `checkResponse(msg)`: verifies that the response's transaction ID matches the request's transaction ID before accepting it.
- `onResponse()`: infers the class (`SuccessResponse`, `ErrorResponse`, or `Indication`) from the response and routes to the correct base class handler.

#### Timeout and retries

The default timeout is 10 seconds with 1 retry. For TURN allocations, the TURN client manages transactions internally via `Client::createTransaction()` and `Client::sendAuthenticatedTransaction()`. Direct use of `stun::Transaction` is only necessary when building a custom STUN-only client (e.g. a standalone NAT discovery tool).

### ICE Attribute Support

The module includes all four ICE-specific attributes needed for connectivity checks and role negotiation:

| Attribute | Type code | Base type | RFC |
| --- | --- | --- | --- |
| `ICEPriority` | `0x0024` | `UInt32Attribute` | 8445 |
| `ICEUseCandidate` | `0x0025` | `FlagAttribute` | 8445 |
| `ICEControlling` | `0x802A` | `UInt64Attribute` | 8445 |
| `ICEControlled` | `0x8029` | `UInt64Attribute` | 8445 |

In Icey's WebRTC stack, libdatachannel/libjuice handles ICE connectivity checks internally. We expose these attribute types so any custom ICE implementation built directly on the STUN module can reuse the same codec rather than duplicating the type definitions.

`ICEControlling` and `ICEControlled` carry a 64-bit tie-breaker value used to resolve ICE role conflicts. `ICEUseCandidate` is a flag; its presence in a connectivity check request nominates the candidate pair. `ICEPriority` carries the candidate priority computed from type preference, local preference, and component ID as specified in RFC 8445 section 5.1.2.

```cpp
// Reading ICE attributes from a received Binding request
auto* prio = msg.get<stun::ICEPriority>();
if (prio) {
    uint32_t p = prio->value();
}

auto* controlling = msg.get<stun::ICEControlling>();
if (controlling) {
    uint64_t tieBreaker = controlling->value();
}

bool isNominating = (msg.get<stun::ICEUseCandidate>() != nullptr);
```

## Samples

The STUN module has no standalone samples. See the [TURN turnserver sample](../../src/turn/samples/turnserver/) for a complete example that builds STUN responses, signs them with `MessageIntegrity`, and handles the full 401 challenge/re-auth flow.

## Configuration

The STUN module has no runtime configuration; all behaviour is determined by the attributes you add to messages and the keys you provide to `MessageIntegrity`. Build-time options:

| CMake option | Default | Effect |
| --- | --- | --- |
| `BUILD_SHARED_LIBS` | `OFF` | Build `icy_stun` as a shared library (DLL on Windows) |
| `BUILD_TESTS` | `OFF` | Build STUN unit tests |

On Windows with `ICY_SHARED_LIBRARY` defined, the `STUN_API` macro expands to `__declspec(dllexport)` in the library and `__declspec(dllimport)` in consumers. On all other platforms it is empty.

## See Also

- [TURN](turn.md) — RFC 5766/6062 relay server and client built on this module
- [Crypto](crypto.md) — `crypto::Hash` used for HMAC key derivation
- [WebRTC](webrtc.md) — WebRTC module that consumes ICE attributes via libdatachannel/libjuice
