# stun 

The `stun` module contains a STUN (rfc5389) implementation.

### Classes

| Name | Description |
|------|-------------|
| [`Attribute`](#classicy_1_1stun_1_1Attribute) | The virtual base class for all STUN/TURN attributes. |
| [`AddressAttribute`](#classicy_1_1stun_1_1AddressAttribute) | Implements a STUN/TURN attribute that contains a socket address. |
| [`UInt8Attribute`](#classicy_1_1stun_1_1UInt8Attribute) | Implements STUN/TURN attribute that reflects a 32-bit integer. |
| [`UInt32Attribute`](#classicy_1_1stun_1_1UInt32Attribute) | Implements STUN/TURN attribute that reflects a 32-bit integer. |
| [`UInt64Attribute`](#classicy_1_1stun_1_1UInt64Attribute) | Implements STUN/TURN attribute that reflects a 64-bit integer. |
| [`FlagAttribute`](#classicy_1_1stun_1_1FlagAttribute) | Implements STUN/TURN attribute representing a 0 size flag. |
| [`StringAttribute`](#classicy_1_1stun_1_1StringAttribute) | Implements STUN/TURN attribute that reflects an arbitrary byte string. |
| [`UInt16ListAttribute`](#classicy_1_1stun_1_1UInt16ListAttribute) | Implements STUN/TURN attribute that reflects a list of attribute names. |
| [`MessageIntegrity`](#classicy_1_1stun_1_1MessageIntegrity) | Implements STUN/TURN attributes that reflects an internet address. |
| [`ErrorCode`](#classicy_1_1stun_1_1ErrorCode) | Implements STUN/TURN attribute that reflects an error code. |
| [`Message`](#classicy_1_1stun_1_1Message) |  |
| [`Transaction`](#classicy_1_1stun_1_1Transaction) |  |

### Members

| Name | Description |
|------|-------------|
| [`STUN_API`](#group__stun_1gac8f9e71a23b67649cae4888b67270410) |  |

---

#### STUN_API 

```cpp
STUN_API()
```

## Attribute 

> **Subclasses:** `icy::stun::AddressAttribute`, `icy::stun::ErrorCode`, `icy::stun::FlagAttribute`, `icy::stun::MessageIntegrity`, `icy::stun::StringAttribute`, `icy::stun::UInt16ListAttribute`, `icy::stun::UInt32Attribute`, `icy::stun::UInt64Attribute`, `icy::stun::UInt8Attribute`
> **Defined in:** `attributes.h`

The virtual base class for all STUN/TURN attributes.

### Members

| Name | Description |
|------|-------------|
| [`Type`](#group__stun_1ga58f9974b99873c21b541593d70751f76) |  |
| [`~Attribute`](#group__stun_1ga3d20ce1b5f294cccb1ceecfd7dde685b) |  |
| [`clone`](#group__stun_1gac70507d30dee5fc90020e5b4a13b5762) |  |
| [`read`](#group__stun_1ga7a8eaf6fae7ce506dead82c972fee53d) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1ga57e4567afce55ae046a19e20d5c96cb8) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`type`](#group__stun_1ga327f31a9e5e89e630fa29bf4d48476ea) |  |
| [`size`](#group__stun_1gaafbe6576c2b87a33cf0faceec8da4e34) |  |
| [`consumePadding`](#group__stun_1ga49af5c5548066acb3de3553218ba04e4) |  |
| [`writePadding`](#group__stun_1ga9337ef7bb7032ea501938e7713f7bc99) |  |
| [`typeString`](#group__stun_1ga11de2203671a16db2e059abaade0d148) |  |
| [`create`](#group__stun_1ga24c455ae024130e437d90515a1ab07bb) | Creates an attribute object with the given type and size. |
| [`typeString`](#group__stun_1ga8f507e68f6b2d62ac7ea3e44cd620601) |  |
| [`Attribute`](#group__stun_1ga9e161b5d779864cb7b8127b5bef9d7e0) |  |
| [`setLength`](#group__stun_1ga5958b14f8dc45b50f2c3183fc1d120bc) |  |
| [`TypeID`](#group__stun_1ga6abc429b06d52e8873d4cd1a20e0ab44) |  |
| [`_type`](#group__stun_1gab338ca1d4ab28668a4c639ec33b31e7c) |  |
| [`_size`](#group__stun_1ga33b3b945c5f3d949e63c24506da36a72) |  |

---

#### Type 

```cpp
enum Type
```

| Value | Description |
|-------|-------------|
| `NotExist` |  |
| `MappedAddress` |  |
| `ResponseAddress` |  |
| `ChangeRequest` |  |
| `SourceAddress` |  |
| `ChangedAddress` |  |
| `Username` |  |
| `Password` |  |
| `MessageIntegrity` |  |
| `ErrorCode` |  |
| `Bandwidth` |  |
| `DestinationAddress` |  |
| `UnknownAttributes` |  |
| `ReflectedFrom` |  |
| `MagicCookie` |  |
| `Realm` |  |
| `Nonce` |  |
| `XorMappedAddress` |  |
| `Software` |  |
| `Options` |  |
| `AlternateServer` |  |
| `Fingerprint` |  |
| `ChannelNumber` | TURN. |
| `Lifetime` |  |
| `XorPeerAddress` |  |
| `Data` |  |
| `XorRelayedAddress` |  |
| `EventPort` |  |
| `RequestedTransport` |  |
| `DontFragment` |  |
| `ReservationToken` | 0x0021: Reserved (was TIMER-VAL) |
| `ConnectionID` | TURN TCP. |
| `ICEControlled` | ICE. |
| `ICEControlling` |  |
| `ICEPriority` |  |
| `ICEUseCandidate` |  |

---

#### ~Attribute 

```cpp
virtual inline ~Attribute()
```

---

#### clone 

```cpp
std::unique_ptr< Attribute > clone()
```

---

#### read 

```cpp
void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### type 

```cpp
uint16_t type() const
```

---

#### size 

```cpp
uint16_t size() const
```

---

#### consumePadding 

```cpp
void consumePadding(BitReader & reader) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### writePadding 

```cpp
void writePadding(BitWriter & writer) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### typeString 

```cpp
std::string typeString()
```

---

#### create 

```cpp
static std::unique_ptr< Attribute > create(uint16_t type, uint16_t size)
```

Creates an attribute object with the given type and size.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

#### typeString 

```cpp
static std::string typeString(uint16_t type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

#### Attribute 

```cpp
Attribute(uint16_t type, uint16_t size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

#### setLength 

```cpp
void setLength(uint16_t size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `uint16_t` |  |

---

#### TypeID 

```cpp
const uint16_t TypeID = 0
```

---

#### _type 

```cpp
uint16_t _type
```

---

#### _size 

```cpp
uint16_t _size
```

## AddressAttribute 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements a STUN/TURN attribute that contains a socket address.

### Members

| Name | Description |
|------|-------------|
| [`AddressAttribute`](#group__stun_1ga26532e946f35dd12074567cb9aa83a89) |  |
| [`AddressAttribute`](#group__stun_1gab1889e0952a12fef4df481639ff5dfd6) |  |
| [`clone`](#group__stun_1ga08f80db0eca11594774aa981fe087951) |  |
| [`family`](#group__stun_1gaa211f6fa23b641e1c922595b1884c287) |  |
| [`address`](#group__stun_1gae694a695a83a7310bb2c2da8e504353c) |  |
| [`read`](#group__stun_1ga4277615adc80ec240e36aa21d198136a) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1gad48cd7a0d308a23c0c4c4549bf3d6fbf) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`setAddress`](#group__stun_1ga6c71d6b0ab7739e63ef2eb58241501c1) |  |
| [`IPv4Size`](#group__stun_1gafb2012ed9f02a84dad86e64a623c3369) |  |
| [`IPv6Size`](#group__stun_1ga4a8505f95e92cbca1efdf9baaaf5e5c5) |  |
| [`_address`](#group__stun_1gad05adfb619be3a9d6977eece3ecf1daa) |  |

---

#### AddressAttribute 

```cpp
AddressAttribute(uint16_t type, bool ipv4)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `ipv4` | `bool` |  |

---

#### AddressAttribute 

```cpp
AddressAttribute(const AddressAttribute & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const AddressAttribute &` |  |

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### family 

```cpp
inline stun::AddressFamily family() const
```

---

#### address 

```cpp
virtual net::Address address() const
```

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### setAddress 

```cpp
virtual inline void setAddress(const net::Address & addr)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const net::Address &` |  |

---

#### IPv4Size 

```cpp
const uint16_t IPv4Size = 8
```

---

#### IPv6Size 

```cpp
const uint16_t IPv6Size = 20
```

---

#### _address 

```cpp
net::Address _address
```

## UInt8Attribute 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attribute that reflects a 32-bit integer.

### Members

| Name | Description |
|------|-------------|
| [`UInt8Attribute`](#group__stun_1ga31a7ecc9da1ab6bb9aef4f84f912644c) |  |
| [`UInt8Attribute`](#group__stun_1gae1a4c7c1a53211e52739bd68955a26dd) |  |
| [`clone`](#group__stun_1ga285944fdf39bff63efd31a10d587769b) |  |
| [`value`](#group__stun_1gaa2111055d94bd81a601e00397e45cc92) |  |
| [`setValue`](#group__stun_1ga222451185a8a5a7f5430d02b76428ee6) |  |
| [`getBit`](#group__stun_1gae230c8a7751d5edfd007cd34b6f56cca) |  |
| [`setBit`](#group__stun_1ga05fb3927bbbb041bd82e06473b686cb3) |  |
| [`read`](#group__stun_1gaab5390f9fbef3b2e7d5b64d65cb13fc7) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1ga632e2485387980cf23a0761705b68a5f) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`Size`](#group__stun_1gaeb8f25eb585791829d527b726e2c2e84) |  |
| [`_bits`](#group__stun_1gaa1803741db41034c1462a17f99a17633) |  |

---

#### UInt8Attribute 

```cpp
UInt8Attribute(uint16_t type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

#### UInt8Attribute 

```cpp
UInt8Attribute(const UInt8Attribute & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const UInt8Attribute &` |  |

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### value 

```cpp
inline uint8_t value() const
```

---

#### setValue 

```cpp
inline void setValue(uint8_t bits)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bits` | `uint8_t` |  |

---

#### getBit 

```cpp
bool getBit(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### setBit 

```cpp
void setBit(int index, bool value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `bool` |  |

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### Size 

```cpp
const uint16_t Size = 1
```

---

#### _bits 

```cpp
uint8_t _bits
```

## UInt32Attribute 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attribute that reflects a 32-bit integer.

### Members

| Name | Description |
|------|-------------|
| [`UInt32Attribute`](#group__stun_1ga3a756731e38a290ec92b77ac9b3dbc78) |  |
| [`UInt32Attribute`](#group__stun_1ga0bf66ed373b22ff7629d82410fa4209a) |  |
| [`clone`](#group__stun_1ga50ffdd5a4e24b8049bdd98fc0f9a578f) |  |
| [`value`](#group__stun_1ga7a2fd11c9ca9b6c27f16efe46b0adfbd) |  |
| [`setValue`](#group__stun_1gaf2e4cb4c7d16d53650c1a061c744472f) |  |
| [`getBit`](#group__stun_1gabe7f310ea52d35848ff63ae5e02ef153) |  |
| [`setBit`](#group__stun_1ga9745eaf379f6449f0a9aa3723cb342f2) |  |
| [`read`](#group__stun_1ga336e05c124918d3e9046fb1b2f66cf09) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1gaf41984e4e8cf61d64d88b1dfbe6e78c7) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`Size`](#group__stun_1ga3bd72ce4cd5089c44bdd15cbb651b656) |  |
| [`_bits`](#group__stun_1gaa5deff7c7ea8a916ff518efb848834b3) |  |

---

#### UInt32Attribute 

```cpp
UInt32Attribute(uint16_t type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

#### UInt32Attribute 

```cpp
UInt32Attribute(const UInt32Attribute & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const UInt32Attribute &` |  |

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### value 

```cpp
inline uint32_t value() const
```

---

#### setValue 

```cpp
inline void setValue(uint32_t bits)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bits` | `uint32_t` |  |

---

#### getBit 

```cpp
bool getBit(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### setBit 

```cpp
void setBit(int index, bool value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `bool` |  |

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### Size 

```cpp
const uint16_t Size = 4
```

---

#### _bits 

```cpp
uint32_t _bits
```

## UInt64Attribute 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attribute that reflects a 64-bit integer.

### Members

| Name | Description |
|------|-------------|
| [`UInt64Attribute`](#group__stun_1ga65fc4d0c4d7140af12a61457b0a89c9e) |  |
| [`UInt64Attribute`](#group__stun_1gae47d568c25008f4d3a6f85c540b0d3bd) |  |
| [`clone`](#group__stun_1ga9903c1b451344bba8e6dd41402e9217d) |  |
| [`value`](#group__stun_1ga986e86dcf77f87c37e4b0ef5cd02ae48) |  |
| [`setValue`](#group__stun_1gaac2af4e0ddd4d677e02c4cae2d3360f5) |  |
| [`getBit`](#group__stun_1ga6de43e36aa136e9b6a14e9821b5038e7) |  |
| [`setBit`](#group__stun_1ga48748faff06ececd74ef35a1410ca2f6) |  |
| [`read`](#group__stun_1ga33044c201fbeac3902f69cb7a6ec05f5) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1ga286f0a969b1a57a3587e04b630a783ce) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`Size`](#group__stun_1ga272ae90692c75840b76ec27cc84ce594) |  |
| [`_bits`](#group__stun_1gae4178ca4345081da2f7f3c610458069e) |  |

---

#### UInt64Attribute 

```cpp
UInt64Attribute(uint16_t type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

#### UInt64Attribute 

```cpp
UInt64Attribute(const UInt64Attribute & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const UInt64Attribute &` |  |

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### value 

```cpp
inline uint64_t value() const
```

---

#### setValue 

```cpp
inline void setValue(uint64_t bits)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bits` | `uint64_t` |  |

---

#### getBit 

```cpp
bool getBit(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### setBit 

```cpp
void setBit(int index, bool value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `bool` |  |

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### Size 

```cpp
const uint16_t Size = 8
```

---

#### _bits 

```cpp
uint64_t _bits
```

## FlagAttribute 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attribute representing a 0 size flag.

### Members

| Name | Description |
|------|-------------|
| [`FlagAttribute`](#group__stun_1ga3d77e3070134506d8f106ee70e09ac27) |  |
| [`clone`](#group__stun_1ga46ddc45c2f45e6e3cf5df8beaadd66d0) |  |
| [`read`](#group__stun_1ga797d67dc26d1b0eb52ff1b995b410200) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1ga0397db5d05fc7fe0ae4a6877ccf1a37d) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`Size`](#group__stun_1gab159d8904434b86d4113e3a8ca6ef509) |  |

---

#### FlagAttribute 

```cpp
FlagAttribute(uint16_t type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### read 

```cpp
virtual inline void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual inline void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### Size 

```cpp
const uint16_t Size = 0
```

## StringAttribute 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attribute that reflects an arbitrary byte string.

### Members

| Name | Description |
|------|-------------|
| [`StringAttribute`](#group__stun_1gadc6a297cfcfa9c94fddef844138837ab) |  |
| [`StringAttribute`](#group__stun_1ga6e51e83ff47fc75c46e8da2f87c338bf) |  |
| [`~StringAttribute`](#group__stun_1ga44630835710accc66471ff5b43c19e87) |  |
| [`clone`](#group__stun_1gab8166601b80abb25c2e4e6c5444680ea) |  |
| [`bytes`](#group__stun_1ga3994fa1ebbe5dfd6e2814de47da07f3d) |  |
| [`setBytes`](#group__stun_1ga4ed88f8e4e00fbd0ba5fd601d5ff55e5) |  |
| [`asString`](#group__stun_1ga88fca368c8e188b41114f4cdea79e79b) |  |
| [`copyBytes`](#group__stun_1gace004bc4eda7af876d91f1b7e5a01d02) |  |
| [`copyBytes`](#group__stun_1ga3480d63f8b4d93697092fba0bdfd2e4d) |  |
| [`getByte`](#group__stun_1ga8ad00fc7dfc7f4661ca2017271f4125b) |  |
| [`setByte`](#group__stun_1gac45f8d1e10aeb2806f4fed85e6f8cd37) |  |
| [`read`](#group__stun_1gabbb6dc3ce9fce25efb2225806a87cb80) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1ga1e9de7ae89ad743833a88413b09921d2) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`_bytes`](#group__stun_1ga39143ccdeb16edc0f5b38f471a2c0c1d) |  |

---

#### StringAttribute 

```cpp
StringAttribute(uint16_t type, uint16_t size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

#### StringAttribute 

```cpp
StringAttribute(const StringAttribute & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const StringAttribute &` |  |

---

#### ~StringAttribute 

```cpp
virtual ~StringAttribute()
```

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### bytes 

```cpp
inline const char * bytes() const
```

---

#### setBytes 

```cpp
void setBytes(const char * bytes, unsigned size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const char *` |  |
| `size` | `unsigned` |  |

---

#### asString 

```cpp
std::string asString() const
```

---

#### copyBytes 

```cpp
void copyBytes(const char * bytes)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const char *` |  |

---

#### copyBytes 

```cpp
void copyBytes(const void * bytes, unsigned size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const void *` |  |
| `size` | `unsigned` |  |

---

#### getByte 

```cpp
uint8_t getByte(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### setByte 

```cpp
void setByte(int index, uint8_t value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `uint8_t` |  |

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### _bytes 

```cpp
std::vector< char > _bytes
```

## UInt16ListAttribute 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attribute that reflects a list of attribute names.

### Members

| Name | Description |
|------|-------------|
| [`UInt16ListAttribute`](#group__stun_1ga30618de614e5bcccd94b2eb3476f5150) |  |
| [`UInt16ListAttribute`](#group__stun_1ga41111abd042a5eabd072a592e423a293) |  |
| [`~UInt16ListAttribute`](#group__stun_1gacc94c85b8a1b73a30b6a764e40685940) |  |
| [`clone`](#group__stun_1gade90ae784fb78df631002185d7d73f6a) |  |
| [`size`](#group__stun_1ga804d0d2ac328075bd20e570ff9e7092b) |  |
| [`getType`](#group__stun_1ga59b9881a1c5ed1c516134aa9d7961fac) |  |
| [`setType`](#group__stun_1ga404efd93fa166215b2181097290cb6eb) |  |
| [`addType`](#group__stun_1gacd1c6bcf94ad054a67910a38da04f803) |  |
| [`read`](#group__stun_1gad0591118478c524485107997abfef834) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1ga7807f8a64746aa49a1a81d9c494de1d6) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`_attrTypes`](#group__stun_1ga2b6dbf667c7b48e8bab3002037438f9c) |  |

---

#### UInt16ListAttribute 

```cpp
UInt16ListAttribute(uint16_t type, uint16_t size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

#### UInt16ListAttribute 

```cpp
UInt16ListAttribute(const UInt16ListAttribute & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const UInt16ListAttribute &` |  |

---

#### ~UInt16ListAttribute 

```cpp
virtual ~UInt16ListAttribute()
```

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### size 

```cpp
size_t size() const
```

---

#### getType 

```cpp
uint16_t getType(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### setType 

```cpp
void setType(int index, uint16_t value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `uint16_t` |  |

---

#### addType 

```cpp
void addType(uint16_t value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint16_t` |  |

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### _attrTypes 

```cpp
std::vector< uint16_t > _attrTypes
```

## MessageIntegrity 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attributes that reflects an internet address.

### Members

| Name | Description |
|------|-------------|
| [`MessageIntegrity`](#group__stun_1ga6d3a6a3407b970154055dbe446670c8f) |  |
| [`MessageIntegrity`](#group__stun_1ga0694d43ce6dc45ba4a11feffcf3c739b) |  |
| [`~MessageIntegrity`](#group__stun_1gac74f0c9b519ad52390a6a4c231f9e60a) |  |
| [`clone`](#group__stun_1ga0f9395fa807db78156ed21da4fd908ae) |  |
| [`verifyHmac`](#group__stun_1gac299a78f8fa192ac0ae022ede8abaad1) |  |
| [`input`](#group__stun_1ga403625dab98075c2b89fe8a65a9c0360) |  |
| [`hmac`](#group__stun_1ga7d79aa318b53e4414af5669f911ec13b) |  |
| [`key`](#group__stun_1ga0f30d8f2c2a6d93893ac5a520bfd9f4f) |  |
| [`setInput`](#group__stun_1ga68372c04e7f849e206c8ceb5f0a19e3b) |  |
| [`setHmac`](#group__stun_1gad71c737937a85fd4af54f6529d30d739) |  |
| [`setKey`](#group__stun_1gaeafda2d185a37768cf3c1537bfa48447) |  |
| [`read`](#group__stun_1gaf313ec3f5940d7dccaa8b9f9789dcedc) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1ga25627d6670f7291f232c0ef65652c3ce) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`TypeID`](#group__stun_1ga4ed5e79ac9774b70be2b0833fbe193c4) |  |
| [`Size`](#group__stun_1ga7f13e95256860a85ed679ea056a7fca1) |  |
| [`_input`](#group__stun_1gac8a5a832e1b3107720359adfecce088a) |  |
| [`_hmac`](#group__stun_1ga77f3a0fbb7cb13ddf7261775c885f5c2) |  |
| [`_key`](#group__stun_1ga7441361a9d0be7ff9d3de74b5b09d81c) |  |

---

#### MessageIntegrity 

```cpp
MessageIntegrity()
```

---

#### MessageIntegrity 

```cpp
MessageIntegrity(const MessageIntegrity & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const MessageIntegrity &` |  |

---

#### ~MessageIntegrity 

```cpp
virtual ~MessageIntegrity()
```

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### verifyHmac 

```cpp
bool verifyHmac(const std::string & key) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### input 

```cpp
inline std::string input() const
```

---

#### hmac 

```cpp
inline std::string hmac() const
```

---

#### key 

```cpp
inline std::string key() const
```

---

#### setInput 

```cpp
inline void setInput(const std::string & input)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `const std::string &` |  |

---

#### setHmac 

```cpp
inline void setHmac(const std::string & hmac)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `hmac` | `const std::string &` |  |

---

#### setKey 

```cpp
inline void setKey(const std::string & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### TypeID 

```cpp
const uint16_t TypeID = 0x0008
```

---

#### Size 

```cpp
const uint16_t Size = 20
```

---

#### _input 

```cpp
std::string _input
```

---

#### _hmac 

```cpp
std::string _hmac
```

---

#### _key 

```cpp
std::string _key
```

## ErrorCode 

> **Extends:** `icy::stun::Attribute`
> **Defined in:** `attributes.h`

Implements STUN/TURN attribute that reflects an error code.

### Members

| Name | Description |
|------|-------------|
| [`ErrorCode`](#group__stun_1gaa1566110dab6203ac5174c1a4a62a8b5) |  |
| [`ErrorCode`](#group__stun_1ga90128dc8b841038c32835371b84afa93) |  |
| [`~ErrorCode`](#group__stun_1gadcf975e8279a398a328a16b75dc29ca5) |  |
| [`clone`](#group__stun_1ga11296179ca068fe44eaa534435b95f21) |  |
| [`setErrorCode`](#group__stun_1gabffcf45778d52bcb75550ca73f7c8f9e) |  |
| [`setReason`](#group__stun_1ga84d94ef041ed8a5e0ddc63547ac86114) |  |
| [`errorCode`](#group__stun_1gaca2120c5893bbf603e0b199a285008ae) |  |
| [`errorClass`](#group__stun_1gaad4ad600c8374950f9416a270b9be653) |  |
| [`errorNumber`](#group__stun_1gab4a6abf000b4c8eb05a10a4ed2576298) |  |
| [`reason`](#group__stun_1ga7fbdfcae77268b78efa51b3c88a06a19) |  |
| [`read`](#group__stun_1ga2ccdf73aeed8dc2e0442f8e3d9078ea6) | Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful. |
| [`write`](#group__stun_1gabf64fa5d609d641a426dedf91bb6b321) | Writes the body (not the type or size) to the given buffer. Return value is true if successful. |
| [`TypeID`](#group__stun_1ga2b266f933243cfc5d1b482c2d9d14418) |  |
| [`MinSize`](#group__stun_1gaf2b0fdc230464a7dcf82ba4fd1a7edfb) |  |
| [`_class`](#group__stun_1gaef16158bf5274f1f6be3cdee05657ff2) |  |
| [`_number`](#group__stun_1gaf197ddb57d24858028593c668bfcf1d8) |  |
| [`_reason`](#group__stun_1ga65fec4754ab028da7cf437a9658de75c) |  |

---

#### ErrorCode 

```cpp
ErrorCode(uint16_t size)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `uint16_t` |  |

---

#### ErrorCode 

```cpp
ErrorCode(const ErrorCode & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const ErrorCode &` |  |

---

#### ~ErrorCode 

```cpp
virtual ~ErrorCode()
```

---

#### clone 

```cpp
virtual std::unique_ptr< Attribute > clone()
```

---

#### setErrorCode 

```cpp
void setErrorCode(int code)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `code` | `int` |  |

---

#### setReason 

```cpp
void setReason(const std::string & reason)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `reason` | `const std::string &` |  |

---

#### errorCode 

```cpp
int errorCode() const
```

---

#### errorClass 

```cpp
inline uint8_t errorClass() const
```

---

#### errorNumber 

```cpp
inline uint8_t errorNumber() const
```

---

#### reason 

```cpp
inline const std::string & reason() const
```

---

#### read 

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `BitReader &` |  |

---

#### write 

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. Return value is true if successful.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `BitWriter &` |  |

---

#### TypeID 

```cpp
const uint16_t TypeID = 0x0009
```

---

#### MinSize 

```cpp
const uint16_t MinSize = 4
```

---

#### _class 

```cpp
uint8_t _class
```

---

#### _number 

```cpp
uint8_t _number
```

---

#### _reason 

```cpp
std::string _reason
```

## Message 

> **Extends:** `icy::IPacket`
> **Subclasses:** `icy::turn::Request`
> **Defined in:** `message.h`

### Members

| Name | Description |
|------|-------------|
| [`MethodType`](#group__stun_1gac2630e3f14c223cdb85801feb8f848ee) |  |
| [`ClassType`](#group__stun_1ga13db6d75080f8b7a57bdb7b7d14cdba6) |  |
| [`ErrorCodes`](#group__stun_1ga4dbaf25761edabeffc85d63482b3aac9) |  |
| [`Message`](#group__stun_1ga87d2d455f130210e6ebdcf10d0318f12) |  |
| [`Message`](#group__stun_1ga19a2f2943326de6b24e4364c136482bf) |  |
| [`Message`](#group__stun_1ga871f9b3b19510574bae25bd42509a1e7) |  |
| [`Message`](#group__stun_1ga3ab2bf6930a6cbe8c847ae0a1c0c374d) |  |
| [`operator=`](#group__stun_1gaa7b3f543c5d432bb2b052820b42d9dbd) |  |
| [`operator=`](#group__stun_1ga3217c09e360934d369b01dbf95edf5b0) |  |
| [`~Message`](#group__stun_1ga4a2d3d7faa8d1fae09689521c719f0e3) |  |
| [`clone`](#group__stun_1gab3b901af3b3812c59a1e6e495f9ff9bd) |  |
| [`setClass`](#group__stun_1gab1616a105946047c77332b9e5caadaac) |  |
| [`setMethod`](#group__stun_1gac7c6555f867fef8dd90c388d13ba2afa) |  |
| [`setTransactionID`](#group__stun_1ga7e1341a7429f6b7199354fda9b235a02) |  |
| [`classType`](#group__stun_1ga7243fa9b4b257cd1ed0a00ae6ed131af) |  |
| [`methodType`](#group__stun_1ga6453d6cb76a0ac955cf7c2d049b8ccce) |  |
| [`transactionID`](#group__stun_1ga7fa4f16f053aa4cefde0941a7cee5696) |  |
| [`size`](#group__stun_1ga6ea0876dd3d70145cdd1944cf257cc89) | The size of the packet in bytes. |
| [`methodString`](#group__stun_1ga6cb46477fe5b8deca78bcfe3be57630c) |  |
| [`classString`](#group__stun_1gae0caeab266e261ca5e0f904fbac29a2a) |  |
| [`errorString`](#group__stun_1ga375de70d1b2e483534b9e152cdaa7e7f) |  |
| [`add`](#group__stun_1ga6d8a672d498981ae3aaf6ae56b64ad57) | Takes ownership of the raw pointer (wraps in unique_ptr). |
| [`add`](#group__stun_1gab062ea939971d108f24ef5a1e83179b4) | Takes ownership via unique_ptr. |
| [`get`](#group__stun_1ga42b45b4a75a1c9a1b7f4e3ee8117e953) |  |
| [`get`](#group__stun_1gae18ed3a5c629f860d1959b2c9a17ac45) |  |
| [`read`](#group__stun_1ga5277c8889063ec2d13b50de692a0f544) | Parses the STUN/TURN packet from the given buffer. The return value indicates the number of bytes read. |
| [`write`](#group__stun_1ga3c6edd2eda4563ce398271ca1e398f2f) | Writes this object into a STUN/TURN packet. |
| [`toString`](#group__stun_1ga50c1ef178929f59e10581ce89f38dbea) |  |
| [`print`](#group__stun_1gab7840803d8273b4343225476ed0c8b2a) |  |
| [`className`](#group__stun_1ga71e77048a4f6e090e42bc441f83ce4c0) |  |
| [`_class`](#group__stun_1gaa6eafc6984888611aa79354a1970d3dd) |  |
| [`_method`](#group__stun_1ga4ae5ed97d42f01ae3556c2b9b4bc242f) |  |
| [`_size`](#group__stun_1ga0f08193c20c1ff33348838fae495f4d7) |  |
| [`_transactionID`](#group__stun_1ga664565d5c2616896d5128d84ca2a7f5f) |  |
| [`_attrs`](#group__stun_1ga00b4d60b33f4d2693640e076f1c0bff3) |  |

---

#### MethodType 

```cpp
enum MethodType
```

| Value | Description |
|-------|-------------|
| `Undefined` | default error type |
| `Binding` | STUN. |
| `Allocate` | TURN. |
| `Refresh` |  |
| `SendIndication` | (only indication semantics defined) |
| `DataIndication` | (only indication semantics defined) |
| `CreatePermission` | (only request/response semantics defined) |
| `ChannelBind` | (only request/response semantics defined) |
| `Connect` | TURN TCP RFC 6062. |
| `ConnectionBind` |  |
| `ConnectionAttempt` |  |

---

#### ClassType 

```cpp
enum ClassType
```

| Value | Description |
|-------|-------------|
| `Request` |  |
| `Indication` |  |
| `SuccessResponse` |  |
| `ErrorResponse` |  |

---

#### ErrorCodes 

```cpp
enum ErrorCodes
```

| Value | Description |
|-------|-------------|
| `BadRequest` |  |
| `NotAuthorized` |  |
| `UnknownAttribute` |  |
| `StaleCredentials` |  |
| `IntegrityCheckFailure` |  |
| `MissingUsername` |  |
| `UseTLS` |  |
| `RoleConflict` |  |
| `ServerError` |  |
| `GlobalFailure` |  |
| `ConnectionAlreadyExists` | TURN TCP. |
| `ConnectionTimeoutOrFailure` |  |

---

#### Message 

```cpp
Message()
```

---

#### Message 

```cpp
Message(ClassType clss, MethodType meth)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `clss` | `ClassType` |  |
| `meth` | `MethodType` |  |

---

#### Message 

```cpp
Message(const Message & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const Message &` |  |

---

#### Message 

```cpp
Message(Message && that) noexcept
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `Message &&` |  |

---

#### operator= 

```cpp
Message & operator=(const Message & that)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const Message &` |  |

---

#### operator= 

```cpp
Message & operator=(Message && that) noexcept
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `Message &&` |  |

---

#### ~Message 

```cpp
virtual ~Message()
```

---

#### clone 

```cpp
virtual IPacket * clone() const
```

---

#### setClass 

```cpp
void setClass(ClassType type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `ClassType` |  |

---

#### setMethod 

```cpp
void setMethod(MethodType type)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `MethodType` |  |

---

#### setTransactionID 

```cpp
void setTransactionID(const std::string & id)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

#### classType 

```cpp
ClassType classType() const
```

---

#### methodType 

```cpp
MethodType methodType() const
```

---

#### transactionID 

```cpp
inline const TransactionID & transactionID() const
```

---

#### size 

```cpp
virtual inline size_t size() const
```

The size of the packet in bytes.

This is the nember of bytes that will be written on a call to [write()](#group__stun_1ga3c6edd2eda4563ce398271ca1e398f2f), but may not be the number of bytes that will be consumed by [read()](#group__stun_1ga5277c8889063ec2d13b50de692a0f544).

---

#### methodString 

```cpp
std::string methodString() const
```

---

#### classString 

```cpp
std::string classString() const
```

---

#### errorString 

```cpp
std::string errorString(uint16_t errorCode) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `errorCode` | `uint16_t` |  |

---

#### add 

```cpp
void add(Attribute * attr)
```

Takes ownership of the raw pointer (wraps in unique_ptr).

| Parameter | Type | Description |
|-----------|------|-------------|
| `attr` | `Attribute *` |  |

---

#### add 

```cpp
void add(std::unique_ptr< Attribute > attr)
```

Takes ownership via unique_ptr.

| Parameter | Type | Description |
|-----------|------|-------------|
| `attr` | `std::unique_ptr< Attribute >` |  |

---

#### get 

```cpp
Attribute * get(Attribute::Type type, int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `Attribute::Type` |  |
| `index` | `int` |  |

---

#### get 

```cpp
template<typename T> inline T * get(int index) const
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

#### read 

```cpp
virtual ssize_t read(const ConstBuffer & buf)
```

Parses the STUN/TURN packet from the given buffer. The return value indicates the number of bytes read.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const ConstBuffer &` |  |

---

#### write 

```cpp
virtual void write(Buffer & buf) const
```

Writes this object into a STUN/TURN packet.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `Buffer &` |  |

---

#### toString 

```cpp
std::string toString() const
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

#### className 

```cpp
virtual inline const char * className() const
```

---

#### _class 

```cpp
uint16_t _class
```

---

#### _method 

```cpp
uint16_t _method
```

---

#### _size 

```cpp
uint16_t _size
```

---

#### _transactionID 

```cpp
TransactionID _transactionID
```

---

#### _attrs 

```cpp
std::vector< std::unique_ptr< Attribute > > _attrs
```

## Transaction 

> **Extends:** `icy::net::Transaction< Message >`
> **Defined in:** `transaction.h`

### Members

| Name | Description |
|------|-------------|
| [`Transaction`](#group__stun_1gaa3eed85a17dc6869a699e7e9c030cca3) |  |
| [`checkResponse`](#group__stun_1ga82ab3d79a18d478172c6b39cc6d7428a) |  |
| [`onResponse`](#group__stun_1gabeecdd054fceca609f3fd449a323159f) | Called when a successful response match is received. |
| [`~Transaction`](#group__stun_1ga8ab19322de3bf9d004ff2dacbcee5daf) |  |

---

#### Transaction 

```cpp
Transaction(const net::Socket::Ptr & socket, const net::Address & peerAddress, long timeout, int retries)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const net::Socket::Ptr &` |  |
| `peerAddress` | `const net::Address &` |  |
| `timeout` | `long` |  |
| `retries` | `int` |  |

---

#### checkResponse 

```cpp
virtual bool checkResponse(const Message & message)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `const Message &` |  |

---

#### onResponse 

```cpp
virtual void onResponse()
```

Called when a successful response match is received.

---

#### ~Transaction 

```cpp
virtual ~Transaction()
```

