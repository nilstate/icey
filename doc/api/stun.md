{#stunmodulerfc5389}

# stun

The `stun` module contains a STUN (rfc5389) implementation.

### Namespaces

| Name | Description |
|------|-------------|
| [`stun`](#stun) |  |

### Macros

| Name | Description |
|------|-------------|
| [`DECLARE_FIXLEN_STUN_ATTRIBUTE`](#declare_fixlen_stun_attribute)  | Attribute macros. |

---

{#declare_fixlen_stun_attribute}

#### DECLARE_FIXLEN_STUN_ATTRIBUTE

```cpp
DECLARE_FIXLEN_STUN_ATTRIBUTE()
```

Attribute macros.

{#stun}

# stun

### Classes

| Name | Description |
|------|-------------|
| [`AddressAttribute`](#addressattribute) | Implements a STUN/TURN attribute that contains a socket address. Handles XOR encoding/decoding for address and port as required by RFC 5389 section 15.2. |
| [`Attribute`](#attribute) | The virtual base class for all STUN/TURN attributes. |
| [`ErrorCode`](#errorcode-1) | Implements the STUN ERROR-CODE attribute (RFC 5389 section 15.6). Encodes a 3-digit error code as a class (hundreds digit) and number (tens + units digits), plus an optional UTF-8 reason phrase. |
| [`FlagAttribute`](#flagattribute) | Implements a zero-length STUN/TURN flag attribute (presence implies the flag is set). |
| [`Message`](#message-5) | STUN/TURN protocol message with method, class, transaction ID, and attributes. |
| [`MessageIntegrity`](#messageintegrity) | Implements the STUN MESSAGE-INTEGRITY attribute (RFC 5389 section 15.4). On write, computes an HMAC-SHA1 over the message bytes preceding this attribute when a key is set. On read, captures the raw HMAC bytes and the input bytes needed to verify them later via [verifyHmac()](#verifyhmac). |
| [`StringAttribute`](#stringattribute) | Implements a STUN/TURN attribute that holds an arbitrary byte string. Used for Username, Password, Realm, Nonce, Software, Data, and similar attributes. |
| [`Transaction`](#transaction-2) | STUN request/response transaction with timeout and retry logic. Extends the generic [net::Transaction](net.md#transaction) with STUN-specific transaction ID matching and response class inference (Success, [Error](base.md#error), or Indication). |
| [`UInt16ListAttribute`](#uint16listattribute) | Implements a STUN/TURN attribute that holds a list of attribute type codes. Used by the UNKNOWN-ATTRIBUTES attribute (RFC 5389 section 15.9). |
| [`UInt32Attribute`](#uint32attribute) | Implements a STUN/TURN attribute that holds a 32-bit integer. |
| [`UInt64Attribute`](#uint64attribute) | Implements a STUN/TURN attribute that holds a 64-bit integer. |
| [`UInt8Attribute`](#uint8attribute) | Implements a STUN/TURN attribute that holds an 8-bit integer. |

### Enumerations

| Name | Description |
|------|-------------|
| [`AddressFamily`](#addressfamily)  | STUN address types as defined in RFC 5389. NB: Undefined is not part of the STUN spec. |

---

{#addressfamily}

#### AddressFamily

```cpp
enum AddressFamily
```

STUN address types as defined in RFC 5389. NB: Undefined is not part of the STUN spec.

| Value | Description |
|-------|-------------|
| `Undefined` |  |
| `IPv4` |  |
| `IPv6` |  |

### Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`TransactionID`](#transactionid-1)  |  |

---

{#transactionid-1}

#### TransactionID

```cpp
std::string TransactionID()
```

### Functions

| Return | Name | Description |
|--------|------|-------------|
| `constexpr bool` | [`isValidMethod`](#isvalidmethod)  | Returns true if `methodType` corresponds to a recognised STUN/TURN method. Used during parsing to reject malformed packets.  |

---

{#isvalidmethod}

#### isValidMethod

```cpp
constexpr bool isValidMethod(uint16_t methodType)
```

Returns true if `methodType` corresponds to a recognised STUN/TURN method. Used during parsing to reject malformed packets. 
#### Parameters
* `methodType` Raw method bits extracted from the message type field. 

#### Returns
true if the method is one of the defined MethodType values.

| Parameter | Type | Description |
|-----------|------|-------------|
| `methodType` | `uint16_t` |  |

### Variables

| Return | Name | Description |
|--------|------|-------------|
| `constexpr int` | [`kAttributeHeaderSize`](#kattributeheadersize)  |  |
| `constexpr int` | [`kMessageHeaderSize`](#kmessageheadersize)  |  |
| `constexpr int` | [`kTransactionIdOffset`](#ktransactionidoffset)  |  |
| `constexpr int` | [`kTransactionIdLength`](#ktransactionidlength)  |  |
| `constexpr uint32_t` | [`kMagicCookie`](#kmagiccookie)  |  |
| `constexpr int` | [`kMagicCookieLength`](#kmagiccookielength)  |  |

---

{#kattributeheadersize}

#### kAttributeHeaderSize

```cpp
constexpr int kAttributeHeaderSize = 4
```

---

{#kmessageheadersize}

#### kMessageHeaderSize

```cpp
constexpr int kMessageHeaderSize = 20
```

---

{#ktransactionidoffset}

#### kTransactionIdOffset

```cpp
constexpr int kTransactionIdOffset = 8
```

---

{#ktransactionidlength}

#### kTransactionIdLength

```cpp
constexpr int kTransactionIdLength = 12
```

---

{#kmagiccookie}

#### kMagicCookie

```cpp
constexpr uint32_t kMagicCookie = 0x2112A442
```

---

{#kmagiccookielength}

#### kMagicCookieLength

```cpp
constexpr int kMagicCookieLength = sizeof()
```

{#addressattribute}

## AddressAttribute

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements a STUN/TURN attribute that contains a socket address. Handles XOR encoding/decoding for address and port as required by RFC 5389 section 15.2.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`AddressAttribute`](#addressattribute-1)  | #### Parameters |
|  | [`AddressAttribute`](#addressattribute-2)  | Copy constructor; duplicates the stored address. |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-7) `virtual` | Returns a deep copy of this attribute. |
| `stun::AddressFamily` | [`family`](#family-2) `const` `inline` | #### Returns |
| `net::Address` | [`address`](#address-12) `virtual` `const` | #### Returns |
| `void` | [`read`](#read-2) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-17) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |
| `void` | [`setAddress`](#setaddress) `virtual` `inline` | Sets the address to encode into this attribute.  |
| `bool` | [`isXorType`](#isxortype) `const` `inline` | #### Returns |

---

{#addressattribute-1}

#### AddressAttribute

```cpp
AddressAttribute(uint16_t type, bool ipv4)
```

#### Parameters
* `type` Wire type code (e.g. XorMappedAddress::TypeID). 

* `ipv4` When true, initialises the size for IPv4; otherwise IPv6.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `ipv4` | `bool` |  |

---

{#addressattribute-2}

#### AddressAttribute

```cpp
AddressAttribute(const AddressAttribute & r)
```

Copy constructor; duplicates the stored address.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [AddressAttribute](#addressattribute) &` |  |

---

{#clone-7}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#family-2}

#### family

`const` `inline`

```cpp
inline stun::AddressFamily family() const
```

#### Returns
The STUN address family (IPv4, IPv6, or Undefined) of the stored address.

---

{#address-12}

#### address

`virtual` `const`

```cpp
virtual net::Address address() const
```

#### Returns
The decoded socket address stored in this attribute.

---

{#read-2}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-17}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

---

{#setaddress}

#### setAddress

`virtual` `inline`

```cpp
virtual inline void setAddress(const net::Address & addr)
```

Sets the address to encode into this attribute. 
#### Parameters
* `addr` Address to store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `addr` | `const [net::Address](net.md#address) &` |  |

---

{#isxortype}

#### isXorType

`const` `inline`

```cpp
inline bool isXorType() const
```

#### Returns
true if this attribute type uses XOR encoding (RFC 5389).

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`IPv4Size`](#ipv4size) `static` |  |
| `constexpr uint16_t` | [`IPv6Size`](#ipv6size) `static` |  |

---

{#ipv4size}

#### IPv4Size

`static`

```cpp
constexpr uint16_t IPv4Size = 8
```

---

{#ipv6size}

#### IPv6Size

`static`

```cpp
constexpr uint16_t IPv6Size = 20
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `net::Address` | [`_address`](#_address-1)  |  |

---

{#_address-1}

#### _address

```cpp
net::Address _address
```

{#attribute}

## Attribute

```cpp
#include <attributes.h>
```

> **Subclassed by:** [`AddressAttribute`](#addressattribute), [`ErrorCode`](#errorcode-1), [`FlagAttribute`](#flagattribute), [`MessageIntegrity`](#messageintegrity), [`StringAttribute`](#stringattribute), [`UInt16ListAttribute`](#uint16listattribute), [`UInt32Attribute`](#uint32attribute), [`UInt64Attribute`](#uint64attribute), [`UInt8Attribute`](#uint8attribute)

The virtual base class for all STUN/TURN attributes.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< Attribute >` | [`clone`](#clone-8)  | Returns a deep copy of this attribute. |
| `void` | [`read`](#read-3)  | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-18) `const` | Writes the body (not the type or size) to the given buffer.  |
| `uint16_t` | [`type`](#type-14) `const` | #### Returns |
| `uint16_t` | [`size`](#size-3) `const` | #### Returns |
| `uint16_t` | [`paddingBytes`](#paddingbytes) `const` `inline` | #### Returns |
| `uint16_t` | [`paddedBytes`](#paddedbytes) `const` `inline` | #### Returns |
| `void` | [`consumePadding`](#consumepadding) `const` | Advances the reader past any 4-byte alignment padding that follows this attribute's body.  |
| `void` | [`writePadding`](#writepadding) `const` | Writes zero-fill padding bytes to align this attribute to a 4-byte boundary.  |
| `std::string` | [`typeString`](#typestring)  | #### Returns |

---

{#clone-8}

#### clone

```cpp
std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#read-3}

#### read

```cpp
void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-18}

#### write

`const`

```cpp
void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

---

{#type-14}

#### type

`const`

```cpp
uint16_t type() const
```

#### Returns
The wire type code for this attribute.

---

{#size-3}

#### size

`const`

```cpp
uint16_t size() const
```

#### Returns
The body length of this attribute in bytes (before padding).

---

{#paddingbytes}

#### paddingBytes

`const` `inline`

```cpp
inline uint16_t paddingBytes() const
```

#### Returns
The 4-byte alignment padding required for this attribute body.

---

{#paddedbytes}

#### paddedBytes

`const` `inline`

```cpp
inline uint16_t paddedBytes() const
```

#### Returns
The body length including 4-byte alignment padding.

---

{#consumepadding}

#### consumePadding

`const`

```cpp
void consumePadding(BitReader & reader) const
```

Advances the reader past any 4-byte alignment padding that follows this attribute's body. 
#### Parameters
* `reader` Reader to advance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#writepadding}

#### writePadding

`const`

```cpp
void writePadding(BitWriter & writer) const
```

Writes zero-fill padding bytes to align this attribute to a 4-byte boundary. 
#### Parameters
* `writer` Writer to append padding to.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

---

{#typestring}

#### typeString

```cpp
std::string typeString()
```

#### Returns
Human-readable name for this attribute's type.

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`TypeID`](#typeid) `static` |  |

---

{#typeid}

#### TypeID

`static`

```cpp
constexpr uint16_t TypeID = 0
```

### Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `std::unique_ptr< Attribute >` | [`create`](#create-8) `static` | Creates an attribute of the given wire type and body size. Returns nullptr if the type is unknown or the size is invalid.  |
| `constexpr uint16_t` | [`paddingBytes`](#paddingbytes-1) `static` `inline` | Returns the 4-byte alignment padding required for a body of `size` bytes. |
| `constexpr uint16_t` | [`paddedBytes`](#paddedbytes-1) `static` `inline` | Returns the body length including 4-byte alignment padding. |
| `std::string` | [`typeString`](#typestring-1) `static` | #### Parameters |

---

{#create-8}

#### create

`static`

```cpp
static std::unique_ptr< Attribute > create(uint16_t type, uint16_t size)
```

Creates an attribute of the given wire type and body size. Returns nullptr if the type is unknown or the size is invalid. 
#### Parameters
* `type` Wire type code (one of [Attribute::Type](#type-15)). 

* `size` Body length in bytes as read from the wire header. 

#### Returns
Owning pointer to the new attribute, or nullptr on failure.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

{#paddingbytes-1}

#### paddingBytes

`static` `inline`

```cpp
static inline constexpr uint16_t paddingBytes(uint16_t size)
```

Returns the 4-byte alignment padding required for a body of `size` bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `uint16_t` |  |

---

{#paddedbytes-1}

#### paddedBytes

`static` `inline`

```cpp
static inline constexpr uint16_t paddedBytes(uint16_t size)
```

Returns the body length including 4-byte alignment padding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `uint16_t` |  |

---

{#typestring-1}

#### typeString

`static`

```cpp
static std::string typeString(uint16_t type)
```

#### Parameters
* `type` Wire type code. 

#### Returns
Human-readable name for the given type code.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint16_t` | [`_type`](#_type-1)  |  |
| `uint16_t` | [`_size`](#_size)  |  |

---

{#_type-1}

#### _type

```cpp
uint16_t _type
```

---

{#_size}

#### _size

```cpp
uint16_t _size
```

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Attribute`](#attribute-1)  | #### Parameters |
| `void` | [`setLength`](#setlength)  | Updates the stored body length.  |

---

{#attribute-1}

#### Attribute

```cpp
Attribute(uint16_t type, uint16_t size)
```

#### Parameters
* `type` Wire type code for this attribute. 

* `size` Initial body length in bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

{#setlength}

#### setLength

```cpp
void setLength(uint16_t size)
```

Updates the stored body length. 
#### Parameters
* `size` New body length in bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `uint16_t` |  |

### Public Types

| Name | Description |
|------|-------------|
| [`Type`](#type-15)  |  |

---

{#type-15}

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

{#errorcode-1}

## ErrorCode

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements the STUN ERROR-CODE attribute (RFC 5389 section 15.6). Encodes a 3-digit error code as a class (hundreds digit) and number (tens + units digits), plus an optional UTF-8 reason phrase.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`ErrorCode`](#errorcode-2)  | #### Parameters |
|  | [`ErrorCode`](#errorcode-3)  | Copy constructor. |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-9) `virtual` | Returns a deep copy of this attribute. |
| `void` | [`setErrorCode`](#seterrorcode)  | Sets the error code, splitting it into class and number fields.  |
| `void` | [`setReason`](#setreason-1)  | Sets the UTF-8 reason phrase and updates the attribute size.  |
| `int` | [`errorCode`](#errorcode-4) `const` | #### Returns |
| `uint8_t` | [`errorClass`](#errorclass) `const` `inline` | #### Returns |
| `uint8_t` | [`errorNumber`](#errornumber-1) `const` `inline` | #### Returns |
| `const std::string &` | [`reason`](#reason-1) `const` `inline` | #### Returns |
| `void` | [`read`](#read-4) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-19) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |

---

{#errorcode-2}

#### ErrorCode

```cpp
ErrorCode(uint16_t size)
```

#### Parameters
* `size` Initial body length in bytes (must be >= MinSize).

| Parameter | Type | Description |
|-----------|------|-------------|
| `size` | `uint16_t` |  |

---

{#errorcode-3}

#### ErrorCode

```cpp
ErrorCode(const ErrorCode & r)
```

Copy constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [ErrorCode](#errorcode-1) &` |  |

---

{#clone-9}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#seterrorcode}

#### setErrorCode

```cpp
void setErrorCode(int code)
```

Sets the error code, splitting it into class and number fields. 
#### Parameters
* `code` 3-digit error code (e.g. 401, 438).

| Parameter | Type | Description |
|-----------|------|-------------|
| `code` | `int` |  |

---

{#setreason-1}

#### setReason

```cpp
void setReason(const std::string & reason)
```

Sets the UTF-8 reason phrase and updates the attribute size. 
#### Parameters
* `reason` Human-readable error description.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reason` | `const std::string &` |  |

---

{#errorcode-4}

#### errorCode

`const`

```cpp
int errorCode() const
```

#### Returns
The full 3-digit error code (class * 100 + number).

---

{#errorclass}

#### errorClass

`const` `inline`

```cpp
inline uint8_t errorClass() const
```

#### Returns
The hundreds digit of the error code (e.g. 4 for a 4xx error).

---

{#errornumber-1}

#### errorNumber

`const` `inline`

```cpp
inline uint8_t errorNumber() const
```

#### Returns
The tens+units portion of the error code (0-99).

---

{#reason-1}

#### reason

`const` `inline`

```cpp
inline const std::string & reason() const
```

#### Returns
The reason phrase string (may be empty).

---

{#read-4}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-19}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`TypeID`](#typeid-1) `static` |  |
| `constexpr uint16_t` | [`MinSize`](#minsize) `static` | 4 bytes before the reason phrase. |

---

{#typeid-1}

#### TypeID

`static`

```cpp
constexpr uint16_t TypeID = 0x0009
```

---

{#minsize}

#### MinSize

`static`

```cpp
constexpr uint16_t MinSize = 4
```

4 bytes before the reason phrase.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint8_t` | [`_class`](#_class)  |  |
| `uint8_t` | [`_number`](#_number)  |  |
| `std::string` | [`_reason`](#_reason-1)  |  |

---

{#_class}

#### _class

```cpp
uint8_t _class
```

---

{#_number}

#### _number

```cpp
uint8_t _number
```

---

{#_reason-1}

#### _reason

```cpp
std::string _reason
```

{#flagattribute}

## FlagAttribute

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements a zero-length STUN/TURN flag attribute (presence implies the flag is set).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`FlagAttribute`](#flagattribute-1)  | #### Parameters |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-10) `virtual` | Returns a deep copy of this attribute. |
| `void` | [`read`](#read-5) `virtual` `inline` | No-op: flag attributes carry no body bytes. |
| `void` | [`write`](#write-20) `virtual` `const` `inline` | No-op: flag attributes carry no body bytes. |

---

{#flagattribute-1}

#### FlagAttribute

```cpp
FlagAttribute(uint16_t type)
```

#### Parameters
* `type` Wire type code for the concrete attribute.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

{#clone-10}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#read-5}

#### read

`virtual` `inline`

```cpp
virtual inline void read(BitReader &)
```

No-op: flag attributes carry no body bytes.

---

{#write-20}

#### write

`virtual` `const` `inline`

```cpp
virtual inline void write(BitWriter &) const
```

No-op: flag attributes carry no body bytes.

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`Size`](#size-4) `static` |  |

---

{#size-4}

#### Size

`static`

```cpp
constexpr uint16_t Size = 0
```

{#message-5}

## Message

```cpp
#include <message.h>
```

> **Inherits:** [`IPacket`](base.md#ipacket)
> **Subclassed by:** [`Request`](turn.md#request-12)

STUN/TURN protocol message with method, class, transaction ID, and attributes.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Message`](#message-6)  | Constructs a default message (Request class, Undefined method) with a randomly generated 12-byte transaction ID. |
|  | [`Message`](#message-7)  | Constructs a message with explicit class and method.  |
|  | [`Message`](#message-8)  | Deep-copy constructor; clones all attributes. |
|  | [`Message`](#message-9)  | Move constructor. |
| `Message &` | [`operator=`](#operator-23)  | Deep-copy assignment; clones all attributes from `that`. |
| `Message &` | [`operator=`](#operator-24)  | Move assignment. |
| `std::unique_ptr< IPacket >` | [`clone`](#clone-11) `virtual` `const` | #### Returns |
| `void` | [`setClass`](#setclass)  | Sets the message class field.  |
| `void` | [`setMethod`](#setmethod-1)  | Sets the message method field.  |
| `void` | [`setTransactionID`](#settransactionid)  | Sets the 12-byte transaction ID.  |
| `ClassType` | [`classType`](#classtype) `const` | #### Returns |
| `MethodType` | [`methodType`](#methodtype) `const` | #### Returns |
| `const TransactionID &` | [`transactionID`](#transactionid-2) `const` `inline` | #### Returns |
| `size_t` | [`size`](#size-5) `virtual` `const` `inline` | #### Returns |
| `std::string` | [`methodString`](#methodstring) `const` | #### Returns |
| `std::string` | [`classString`](#classstring) `const` | #### Returns |
| `std::string` | [`errorString`](#errorstring) `const` | Maps a numeric error code to its canonical string description.  |
| `T &` | [`add`](#add-1) `inline` | Constructs an attribute of type T in-place and appends it to the message. Returns a reference to the new attribute for further configuration.  |
| `void` | [`add`](#add-2)  | Appends an attribute to the message, taking ownership via unique_ptr.  |
| `Attribute *` | [`get`](#get-5) `const` | Returns the Nth attribute of the given type, or nullptr if not found.  |
| `T *` | [`get`](#get-6) `const` `inline` | Type-safe attribute accessor using the concrete attribute's TypeID.  |
| `ssize_t` | [`read`](#read-6) `virtual` | Parses a STUN/TURN packet from the given buffer.  |
| `void` | [`write`](#write-21) `virtual` `const` | Serialises this message into a STUN/TURN wire-format packet.  |
| `std::string` | [`toString`](#tostring-8) `const` | #### Returns |
| `void` | [`print`](#print-13) `virtual` `const` | Writes the same representation as [toString()](#tostring-8) to the given stream.  |
| `const char *` | [`className`](#classname-7) `virtual` `const` `inline` | Returns the class name of this packet type for logging and diagnostics. |

---

{#message-6}

#### Message

```cpp
Message()
```

Constructs a default message (Request class, Undefined method) with a randomly generated 12-byte transaction ID.

---

{#message-7}

#### Message

```cpp
Message(ClassType clss, MethodType meth)
```

Constructs a message with explicit class and method. 
#### Parameters
* `clss` [Message](#message-5) class (Request, Indication, SuccessResponse, or ErrorResponse). 

* `meth` [Message](#message-5) method (Binding, Allocate, Refresh, etc.).

| Parameter | Type | Description |
|-----------|------|-------------|
| `clss` | `[ClassType](#classtype-1)` |  |
| `meth` | `[MethodType](#methodtype-1)` |  |

---

{#message-8}

#### Message

```cpp
Message(const Message & that)
```

Deep-copy constructor; clones all attributes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const [Message](#message-5) &` |  |

---

{#message-9}

#### Message

```cpp
Message(Message && that) noexcept
```

Move constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `[Message](#message-5) &&` |  |

---

{#operator-23}

#### operator=

```cpp
Message & operator=(const Message & that)
```

Deep-copy assignment; clones all attributes from `that`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `const [Message](#message-5) &` |  |

---

{#operator-24}

#### operator=

```cpp
Message & operator=(Message && that) noexcept
```

Move assignment.

| Parameter | Type | Description |
|-----------|------|-------------|
| `that` | `[Message](#message-5) &&` |  |

---

{#clone-11}

#### clone

`virtual` `const`

```cpp
virtual std::unique_ptr< IPacket > clone() const
```

#### Returns
A heap-allocated deep copy of this message.

---

{#setclass}

#### setClass

```cpp
void setClass(ClassType type)
```

Sets the message class field. 
#### Parameters
* `type` One of Request, Indication, SuccessResponse, ErrorResponse.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `[ClassType](#classtype-1)` |  |

---

{#setmethod-1}

#### setMethod

```cpp
void setMethod(MethodType type)
```

Sets the message method field. 
#### Parameters
* `type` One of the MethodType enumerators.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `[MethodType](#methodtype-1)` |  |

---

{#settransactionid}

#### setTransactionID

```cpp
void setTransactionID(const std::string & id)
```

Sets the 12-byte transaction ID. 
#### Parameters
* `id` Must be exactly kTransactionIdLength (12) bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `const std::string &` |  |

---

{#classtype}

#### classType

`const`

```cpp
ClassType classType() const
```

#### Returns
The message class.

---

{#methodtype}

#### methodType

`const`

```cpp
MethodType methodType() const
```

#### Returns
The message method.

---

{#transactionid-2}

#### transactionID

`const` `inline`

```cpp
inline const TransactionID & transactionID() const
```

#### Returns
Reference to the 12-byte transaction ID string.

---

{#size-5}

#### size

`virtual` `const` `inline`

```cpp
virtual inline size_t size() const
```

#### Returns
Total body size in bytes (sum of padded attribute headers and bodies).

---

{#methodstring}

#### methodString

`const`

```cpp
std::string methodString() const
```

#### Returns
Human-readable method name (e.g. "BINDING", "ALLOCATE").

---

{#classstring}

#### classString

`const`

```cpp
std::string classString() const
```

#### Returns
Human-readable class name (e.g. "Request", "SuccessResponse").

---

{#errorstring}

#### errorString

`const`

```cpp
std::string errorString(uint16_t errorCode) const
```

Maps a numeric error code to its canonical string description. 
#### Parameters
* `errorCode` One of the ErrorCodes enumerators. 

#### Returns
Human-readable error string, or "UnknownError" if not recognised.

| Parameter | Type | Description |
|-----------|------|-------------|
| `errorCode` | `uint16_t` |  |

---

{#add-1}

#### add

`inline`

```cpp
template<typename T> inline T & add()
```

Constructs an attribute of type T in-place and appends it to the message. Returns a reference to the new attribute for further configuration. 
#### Parameters
* `T` Concrete attribute type (e.g. stun::Lifetime, stun::XorMappedAddress). 

#### Returns
Reference to the newly added attribute.

---

{#add-2}

#### add

```cpp
void add(std::unique_ptr< Attribute > attr)
```

Appends an attribute to the message, taking ownership via unique_ptr. 
#### Parameters
* `attr` [Attribute](#attribute) to add.

| Parameter | Type | Description |
|-----------|------|-------------|
| `attr` | `std::unique_ptr< [Attribute](#attribute) >` |  |

---

{#get-5}

#### get

`const`

```cpp
Attribute * get(Attribute::Type type, int index) const
```

Returns the Nth attribute of the given type, or nullptr if not found. 
#### Parameters
* `type` [Attribute](#attribute) type code to search for. 

* `index` Zero-based occurrence index (0 = first match). 

#### Returns
Raw pointer to the attribute (owned by this message), or nullptr.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `[Attribute::Type](#type-15)` |  |
| `index` | `int` |  |

---

{#get-6}

#### get

`const` `inline`

```cpp
template<typename T> inline T * get(int index) const
```

Type-safe attribute accessor using the concrete attribute's TypeID. 
#### Parameters
* `T` Concrete attribute type (must define TypeID). 

#### Parameters
* `index` Zero-based occurrence index. 

#### Returns
Pointer to T, or nullptr if the attribute is absent.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

{#read-6}

#### read

`virtual`

```cpp
virtual ssize_t read(const ConstBuffer & buf)
```

Parses a STUN/TURN packet from the given buffer. 
#### Parameters
* `buf` Buffer containing at least one complete STUN message. 

#### Returns
Number of bytes consumed, or 0 on parse failure.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `const [ConstBuffer](base.md#constbuffer) &` |  |

---

{#write-21}

#### write

`virtual` `const`

```cpp
virtual void write(Buffer & buf) const
```

Serialises this message into a STUN/TURN wire-format packet. 
#### Parameters
* `buf` Destination buffer; data is appended.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buf` | `[Buffer](base.md#buffer-2) &` |  |

---

{#tostring-8}

#### toString

`const`

```cpp
std::string toString() const
```

#### Returns
A concise string representation for logging (method, transaction ID, attribute types).

---

{#print-13}

#### print

`virtual` `const`

```cpp
virtual void print(std::ostream & os) const
```

Writes the same representation as [toString()](#tostring-8) to the given stream. 
#### Parameters
* `os` Output stream.

| Parameter | Type | Description |
|-----------|------|-------------|
| `os` | `std::ostream &` |  |

---

{#classname-7}

#### className

`virtual` `const` `inline`

```cpp
virtual inline const char * className() const
```

Returns the class name of this packet type for logging and diagnostics.

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint16_t` | [`_class`](#_class-1)  |  |
| `uint16_t` | [`_method`](#_method-1)  |  |
| `uint16_t` | [`_size`](#_size-1)  | Set by [read()](#read-6); [write()](#write-21) uses computeBodySize() instead. |
| `TransactionID` | [`_transactionID`](#_transactionid)  |  |
| `std::vector< std::unique_ptr< Attribute > >` | [`_attrs`](#_attrs)  |  |

---

{#_class-1}

#### _class

```cpp
uint16_t _class
```

---

{#_method-1}

#### _method

```cpp
uint16_t _method
```

---

{#_size-1}

#### _size

```cpp
uint16_t _size
```

Set by [read()](#read-6); [write()](#write-21) uses computeBodySize() instead.

---

{#_transactionid}

#### _transactionID

```cpp
TransactionID _transactionID
```

---

{#_attrs}

#### _attrs

```cpp
std::vector< std::unique_ptr< Attribute > > _attrs
```

### Public Types

| Name | Description |
|------|-------------|
| [`MethodType`](#methodtype-1)  |  |
| [`ClassType`](#classtype-1)  |  |
| [`ErrorCodes`](#errorcodes)  |  |

---

{#methodtype-1}

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

{#classtype-1}

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

{#errorcodes}

#### ErrorCodes

```cpp
enum ErrorCodes
```

| Value | Description |
|-------|-------------|
| `TryAlternate` |  |
| `BadRequest` |  |
| `NotAuthorized` |  |
| `Forbidden` |  |
| `UnknownAttribute` |  |
| `StaleCredentials` |  |
| `IntegrityCheckFailure` |  |
| `MissingUsername` |  |
| `UseTLS` |  |
| `AllocationMismatch` |  |
| `StaleNonce` |  |
| `WrongCredentials` |  |
| `UnsupportedTransport` |  |
| `AllocationQuotaReached` |  |
| `RoleConflict` |  |
| `ServerError` |  |
| `InsufficientCapacity` |  |
| `GlobalFailure` |  |
| `ConnectionAlreadyExists` | TURN TCP. |
| `ConnectionTimeoutOrFailure` |  |

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `uint16_t` | [`computeBodySize`](#computebodysize) `const` | Computes the wire body size from the current attribute list. |

---

{#computebodysize}

#### computeBodySize

`const`

```cpp
uint16_t computeBodySize() const
```

Computes the wire body size from the current attribute list.

{#messageintegrity}

## MessageIntegrity

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements the STUN MESSAGE-INTEGRITY attribute (RFC 5389 section 15.4). On write, computes an HMAC-SHA1 over the message bytes preceding this attribute when a key is set. On read, captures the raw HMAC bytes and the input bytes needed to verify them later via [verifyHmac()](#verifyhmac).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MessageIntegrity`](#messageintegrity-1)  |  |
|  | [`MessageIntegrity`](#messageintegrity-2)  |  |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-12) `virtual` | Returns a deep copy of this attribute. |
| `bool` | [`verifyHmac`](#verifyhmac) `const` | Verifies the stored HMAC against the stored input bytes using `key`.  |
| `std::string` | [`input`](#input) `const` `inline` | #### Returns |
| `std::string` | [`hmac`](#hmac) `const` `inline` | #### Returns |
| `std::string` | [`key`](#key-2) `const` `inline` | #### Returns |
| `void` | [`setInput`](#setinput) `inline` | Sets the raw message bytes used as HMAC input during verification.  |
| `void` | [`setHmac`](#sethmac) `inline` | Sets the raw HMAC value (used when copying a received attribute).  |
| `void` | [`setKey`](#setkey) `inline` | Sets the HMAC key; triggers HMAC computation on [write()](#write-22).  |
| `void` | [`read`](#read-7) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-22) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |

---

{#messageintegrity-1}

#### MessageIntegrity

```cpp
MessageIntegrity()
```

---

{#messageintegrity-2}

#### MessageIntegrity

```cpp
MessageIntegrity(const MessageIntegrity & r)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [MessageIntegrity](#messageintegrity) &` |  |

---

{#clone-12}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#verifyhmac}

#### verifyHmac

`const`

```cpp
bool verifyHmac(std::string_view key) const
```

Verifies the stored HMAC against the stored input bytes using `key`. 
#### Parameters
* `key` HMAC key (MD5 of username:realm:password for long-term creds). 

#### Returns
true if the computed HMAC matches the stored HMAC.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `std::string_view` |  |

---

{#input}

#### input

`const` `inline`

```cpp
inline std::string input() const
```

#### Returns
The raw message bytes captured at read time, used for HMAC verification.

---

{#hmac}

#### hmac

`const` `inline`

```cpp
inline std::string hmac() const
```

#### Returns
The raw 20-byte HMAC value as read from the wire.

---

{#key-2}

#### key

`const` `inline`

```cpp
inline std::string key() const
```

#### Returns
The HMAC key set for outgoing message signing (empty if not set).

---

{#setinput}

#### setInput

`inline`

```cpp
inline void setInput(const std::string & input)
```

Sets the raw message bytes used as HMAC input during verification. 
#### Parameters
* `input` Byte string of the message up to this attribute.

| Parameter | Type | Description |
|-----------|------|-------------|
| `input` | `const std::string &` |  |

---

{#sethmac}

#### setHmac

`inline`

```cpp
inline void setHmac(const std::string & hmac)
```

Sets the raw HMAC value (used when copying a received attribute). 
#### Parameters
* `hmac` 20-byte HMAC string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `hmac` | `const std::string &` |  |

---

{#setkey}

#### setKey

`inline`

```cpp
inline void setKey(const std::string & key)
```

Sets the HMAC key; triggers HMAC computation on [write()](#write-22). 
#### Parameters
* `key` MD5 digest of the long-term credential (username:realm:password).

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

{#read-7}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-22}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`TypeID`](#typeid-2) `static` |  |
| `constexpr uint16_t` | [`Size`](#size-6) `static` | HMAC-SHA1 output is always 20 bytes. |

---

{#typeid-2}

#### TypeID

`static`

```cpp
constexpr uint16_t TypeID = 0x0008
```

---

{#size-6}

#### Size

`static`

```cpp
constexpr uint16_t Size = 20
```

HMAC-SHA1 output is always 20 bytes.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::string` | [`_input`](#_input)  |  |
| `std::string` | [`_hmac`](#_hmac)  |  |
| `std::string` | [`_key`](#_key-1)  |  |

---

{#_input}

#### _input

```cpp
std::string _input
```

---

{#_hmac}

#### _hmac

```cpp
std::string _hmac
```

---

{#_key-1}

#### _key

```cpp
std::string _key
```

{#stringattribute}

## StringAttribute

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements a STUN/TURN attribute that holds an arbitrary byte string. Used for Username, Password, Realm, Nonce, Software, Data, and similar attributes.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`StringAttribute`](#stringattribute-1)  | #### Parameters |
|  | [`StringAttribute`](#stringattribute-2)  | Copy constructor; duplicates stored bytes. |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-13) `virtual` | Returns a deep copy of this attribute. |
| `const char *` | [`bytes`](#bytes-1) `const` `inline` | #### Returns |
| `void` | [`setBytes`](#setbytes)  | Replaces the stored bytes with a copy of the given buffer and updates the attribute's reported size.  |
| `std::string` | [`asString`](#asstring) `const` | #### Returns |
| `void` | [`copyBytes`](#copybytes)  | Copies a null-terminated string into the attribute, using strlen to determine the length.  |
| `void` | [`copyBytes`](#copybytes-1)  | Copies an arbitrary block of memory into the attribute.  |
| `uint8_t` | [`getByte`](#getbyte) `const` | Returns a single byte from the stored buffer.  |
| `void` | [`setByte`](#setbyte)  | Overwrites a single byte in the stored buffer.  |
| `void` | [`read`](#read-8) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-23) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |

---

{#stringattribute-1}

#### StringAttribute

```cpp
StringAttribute(uint16_t type, uint16_t size)
```

#### Parameters
* `type` Wire type code for the concrete attribute. 

* `size` Initial body length in bytes (0 for variable-length attributes).

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

{#stringattribute-2}

#### StringAttribute

```cpp
StringAttribute(const StringAttribute & r)
```

Copy constructor; duplicates stored bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [StringAttribute](#stringattribute) &` |  |

---

{#clone-13}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#bytes-1}

#### bytes

`const` `inline`

```cpp
inline const char * bytes() const
```

#### Returns
Pointer to the raw byte buffer.

---

{#setbytes}

#### setBytes

```cpp
void setBytes(const char * bytes, unsigned size)
```

Replaces the stored bytes with a copy of the given buffer and updates the attribute's reported size. 
#### Parameters
* `bytes` Source data pointer. 

* `size` Number of bytes to copy.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const char *` |  |
| `size` | `unsigned` |  |

---

{#asstring}

#### asString

`const`

```cpp
std::string asString() const
```

#### Returns
The stored bytes as a std::string.

---

{#copybytes}

#### copyBytes

```cpp
void copyBytes(const char * bytes)
```

Copies a null-terminated string into the attribute, using strlen to determine the length. 
#### Parameters
* `bytes` Null-terminated source string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const char *` |  |

---

{#copybytes-1}

#### copyBytes

```cpp
void copyBytes(const void * bytes, unsigned size)
```

Copies an arbitrary block of memory into the attribute. 
#### Parameters
* `bytes` Source data pointer. 

* `size` Number of bytes to copy.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bytes` | `const void *` |  |
| `size` | `unsigned` |  |

---

{#getbyte}

#### getByte

`const`

```cpp
uint8_t getByte(int index) const
```

Returns a single byte from the stored buffer. 
#### Parameters
* `index` Zero-based byte offset. 

#### Returns
The byte value at `index`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

{#setbyte}

#### setByte

```cpp
void setByte(int index, uint8_t value)
```

Overwrites a single byte in the stored buffer. 
#### Parameters
* `index` Zero-based byte offset. 

* `value` New value to write.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `uint8_t` |  |

---

{#read-8}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-23}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< char >` | [`_bytes`](#_bytes)  |  |

---

{#_bytes}

#### _bytes

```cpp
std::vector< char > _bytes
```

{#transaction-2}

## Transaction

```cpp
#include <transaction.h>
```

> **Inherits:** [`Transaction< Message >`](net.md#transaction)

STUN request/response transaction with timeout and retry logic. Extends the generic [net::Transaction](net.md#transaction) with STUN-specific transaction ID matching and response class inference (Success, [Error](base.md#error), or Indication).

Lifetime is managed by [IntrusivePtr](base.md#intrusiveptr). Create via makeIntrusive or wrap in [IntrusivePtr](base.md#intrusiveptr) immediately after construction.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`Transaction`](#transaction-3)  | Constructs a STUN transaction bound to a specific socket and peer.  |
| `bool` | [`checkResponse`](#checkresponse-1)  | Checks that `message` is a valid response for the pending request. In addition to the base class check, verifies that the transaction IDs match.  |
| `void` | [`onResponse`](#onresponse-1) `virtual` | Called when a valid response is received. Infers the response class (SuccessResponse, ErrorResponse, or Indication) from the response attributes and delegates to the base class handler. |

---

{#transaction-3}

#### Transaction

```cpp
Transaction(const net::Socket::Ptr & socket, const net::Address & peerAddress, long timeout, int retries)
```

Constructs a STUN transaction bound to a specific socket and peer. 
#### Parameters
* `socket` Socket used to send the request and receive the response. 

* `peerAddress` Remote address of the STUN/TURN server. 

* `timeout` Response timeout in milliseconds (default 10 s). 

* `retries` Number of send retries before declaring failure (default 1).

| Parameter | Type | Description |
|-----------|------|-------------|
| `socket` | `const [net::Socket::Ptr](net.md#ptr-4) &` |  |
| `peerAddress` | `const [net::Address](net.md#address) &` |  |
| `timeout` | `long` |  |
| `retries` | `int` |  |

---

{#checkresponse-1}

#### checkResponse

```cpp
bool checkResponse(const Message & message)
```

Checks that `message` is a valid response for the pending request. In addition to the base class check, verifies that the transaction IDs match. 
#### Parameters
* `message` Incoming STUN message to evaluate. 

#### Returns
true if `message` is the expected response.

| Parameter | Type | Description |
|-----------|------|-------------|
| `message` | `const [Message](#message-5) &` |  |

---

{#onresponse-1}

#### onResponse

`virtual`

```cpp
virtual void onResponse()
```

Called when a valid response is received. Infers the response class (SuccessResponse, ErrorResponse, or Indication) from the response attributes and delegates to the base class handler.

### Public Types

| Name | Description |
|------|-------------|
| [`Ptr`](#ptr-16)  |  |

---

{#ptr-16}

#### Ptr

```cpp
IntrusivePtr< Transaction > Ptr()
```

{#uint16listattribute}

## UInt16ListAttribute

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements a STUN/TURN attribute that holds a list of attribute type codes. Used by the UNKNOWN-ATTRIBUTES attribute (RFC 5389 section 15.9).

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`UInt16ListAttribute`](#uint16listattribute-1)  | #### Parameters |
|  | [`UInt16ListAttribute`](#uint16listattribute-2)  | Copy constructor; duplicates the type list. |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-14) `virtual` | Returns a deep copy of this attribute. |
| `size_t` | [`size`](#size-7) `const` | #### Returns |
| `uint16_t` | [`getType`](#gettype) `const` | Returns the type code at the given list position.  |
| `void` | [`setType`](#settype)  | Overwrites the type code at the given list position.  |
| `void` | [`addType`](#addtype)  | Appends a type code to the list and updates the attribute size.  |
| `void` | [`read`](#read-9) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-24) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |

---

{#uint16listattribute-1}

#### UInt16ListAttribute

```cpp
UInt16ListAttribute(uint16_t type, uint16_t size)
```

#### Parameters
* `type` Wire type code for the concrete attribute. 

* `size` Initial body length in bytes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |
| `size` | `uint16_t` |  |

---

{#uint16listattribute-2}

#### UInt16ListAttribute

```cpp
UInt16ListAttribute(const UInt16ListAttribute & r)
```

Copy constructor; duplicates the type list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [UInt16ListAttribute](#uint16listattribute) &` |  |

---

{#clone-14}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#size-7}

#### size

`const`

```cpp
size_t size() const
```

#### Returns
Number of attribute type codes in the list.

---

{#gettype}

#### getType

`const`

```cpp
uint16_t getType(int index) const
```

Returns the type code at the given list position. 
#### Parameters
* `index` Zero-based list index. 

#### Returns
[Attribute](#attribute) type code at `index`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

{#settype}

#### setType

```cpp
void setType(int index, uint16_t value)
```

Overwrites the type code at the given list position. 
#### Parameters
* `index` Zero-based list index. 

* `value` New attribute type code.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `uint16_t` |  |

---

{#addtype}

#### addType

```cpp
void addType(uint16_t value)
```

Appends a type code to the list and updates the attribute size. 
#### Parameters
* `value` [Attribute](#attribute) type code to append.

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint16_t` |  |

---

{#read-9}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-24}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::vector< uint16_t >` | [`_attrTypes`](#_attrtypes)  |  |

---

{#_attrtypes}

#### _attrTypes

```cpp
std::vector< uint16_t > _attrTypes
```

{#uint32attribute}

## UInt32Attribute

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements a STUN/TURN attribute that holds a 32-bit integer.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`UInt32Attribute`](#uint32attribute-1)  | #### Parameters |
|  | [`UInt32Attribute`](#uint32attribute-2)  | Copy constructor. |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-15) `virtual` | Returns a deep copy of this attribute. |
| `uint32_t` | [`value`](#value-1) `const` `inline` | #### Returns |
| `void` | [`setValue`](#setvalue-1) `inline` | Sets the stored 32-bit value.  |
| `bool` | [`getBit`](#getbit) `const` | Returns the state of a single bit within the stored word.  |
| `void` | [`setBit`](#setbit)  | Sets or clears a single bit within the stored word.  |
| `void` | [`read`](#read-10) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-25) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |

---

{#uint32attribute-1}

#### UInt32Attribute

```cpp
UInt32Attribute(uint16_t type)
```

#### Parameters
* `type` Wire type code for the concrete attribute.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

{#uint32attribute-2}

#### UInt32Attribute

```cpp
UInt32Attribute(const UInt32Attribute & r)
```

Copy constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [UInt32Attribute](#uint32attribute) &` |  |

---

{#clone-15}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#value-1}

#### value

`const` `inline`

```cpp
inline uint32_t value() const
```

#### Returns
The stored 32-bit value.

---

{#setvalue-1}

#### setValue

`inline`

```cpp
inline void setValue(uint32_t bits)
```

Sets the stored 32-bit value. 
#### Parameters
* `bits` Value to store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bits` | `uint32_t` |  |

---

{#getbit}

#### getBit

`const`

```cpp
bool getBit(int index) const
```

Returns the state of a single bit within the stored word. 
#### Parameters
* `index` Bit position (0 = LSB, 31 = MSB). 

#### Returns
true if the bit is set.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

{#setbit}

#### setBit

```cpp
void setBit(int index, bool value)
```

Sets or clears a single bit within the stored word. 
#### Parameters
* `index` Bit position (0 = LSB, 31 = MSB). 

* `value` true to set, false to clear.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `bool` |  |

---

{#read-10}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-25}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`Size`](#size-8) `static` |  |

---

{#size-8}

#### Size

`static`

```cpp
constexpr uint16_t Size = 4
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`_bits`](#_bits)  |  |

---

{#_bits}

#### _bits

```cpp
uint32_t _bits
```

{#uint64attribute}

## UInt64Attribute

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements a STUN/TURN attribute that holds a 64-bit integer.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`UInt64Attribute`](#uint64attribute-1)  | #### Parameters |
|  | [`UInt64Attribute`](#uint64attribute-2)  | Copy constructor. |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-16) `virtual` | Returns a deep copy of this attribute. |
| `uint64_t` | [`value`](#value-2) `const` `inline` | #### Returns |
| `void` | [`setValue`](#setvalue-2) `inline` | Sets the stored 64-bit value.  |
| `bool` | [`getBit`](#getbit-1) `const` | Returns the state of a single bit within the stored quad-word.  |
| `void` | [`setBit`](#setbit-1)  | Sets or clears a single bit within the stored quad-word.  |
| `void` | [`read`](#read-11) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-26) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |

---

{#uint64attribute-1}

#### UInt64Attribute

```cpp
UInt64Attribute(uint16_t type)
```

#### Parameters
* `type` Wire type code for the concrete attribute.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

{#uint64attribute-2}

#### UInt64Attribute

```cpp
UInt64Attribute(const UInt64Attribute & r)
```

Copy constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [UInt64Attribute](#uint64attribute) &` |  |

---

{#clone-16}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#value-2}

#### value

`const` `inline`

```cpp
inline uint64_t value() const
```

#### Returns
The stored 64-bit value.

---

{#setvalue-2}

#### setValue

`inline`

```cpp
inline void setValue(uint64_t bits)
```

Sets the stored 64-bit value. 
#### Parameters
* `bits` Value to store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bits` | `uint64_t` |  |

---

{#getbit-1}

#### getBit

`const`

```cpp
bool getBit(int index) const
```

Returns the state of a single bit within the stored quad-word. 
#### Parameters
* `index` Bit position (0 = LSB, 63 = MSB). 

#### Returns
true if the bit is set.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

{#setbit-1}

#### setBit

```cpp
void setBit(int index, bool value)
```

Sets or clears a single bit within the stored quad-word. 
#### Parameters
* `index` Bit position (0 = LSB, 63 = MSB). 

* `value` true to set, false to clear.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `bool` |  |

---

{#read-11}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-26}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`Size`](#size-9) `static` |  |

---

{#size-9}

#### Size

`static`

```cpp
constexpr uint16_t Size = 8
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`_bits`](#_bits-1)  |  |

---

{#_bits-1}

#### _bits

```cpp
uint64_t _bits
```

{#uint8attribute}

## UInt8Attribute

```cpp
#include <attributes.h>
```

> **Inherits:** [`Attribute`](#attribute)

Implements a STUN/TURN attribute that holds an 8-bit integer.

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`UInt8Attribute`](#uint8attribute-1)  | #### Parameters |
|  | [`UInt8Attribute`](#uint8attribute-2)  | Copy constructor. |
| `std::unique_ptr< Attribute >` | [`clone`](#clone-17) `virtual` | Returns a deep copy of this attribute. |
| `uint8_t` | [`value`](#value-3) `const` `inline` | #### Returns |
| `void` | [`setValue`](#setvalue-3) `inline` | Sets the stored 8-bit value.  |
| `bool` | [`getBit`](#getbit-2) `const` | Returns the state of a single bit within the stored byte.  |
| `void` | [`setBit`](#setbit-2)  | Sets or clears a single bit within the stored byte.  |
| `void` | [`read`](#read-12) `virtual` | Reads the body (not the type or size) for this type of attribute from the given buffer.  |
| `void` | [`write`](#write-27) `virtual` `const` | Writes the body (not the type or size) to the given buffer.  |

---

{#uint8attribute-1}

#### UInt8Attribute

```cpp
UInt8Attribute(uint16_t type)
```

#### Parameters
* `type` Wire type code for the concrete attribute.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | `uint16_t` |  |

---

{#uint8attribute-2}

#### UInt8Attribute

```cpp
UInt8Attribute(const UInt8Attribute & r)
```

Copy constructor.

| Parameter | Type | Description |
|-----------|------|-------------|
| `r` | `const [UInt8Attribute](#uint8attribute) &` |  |

---

{#clone-17}

#### clone

`virtual`

```cpp
virtual std::unique_ptr< Attribute > clone()
```

Returns a deep copy of this attribute.

---

{#value-3}

#### value

`const` `inline`

```cpp
inline uint8_t value() const
```

#### Returns
The stored 8-bit value.

---

{#setvalue-3}

#### setValue

`inline`

```cpp
inline void setValue(uint8_t bits)
```

Sets the stored 8-bit value. 
#### Parameters
* `bits` Value to store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bits` | `uint8_t` |  |

---

{#getbit-2}

#### getBit

`const`

```cpp
bool getBit(int index) const
```

Returns the state of a single bit within the stored byte. 
#### Parameters
* `index` Bit position (0 = LSB, 7 = MSB). 

#### Returns
true if the bit is set.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |

---

{#setbit-2}

#### setBit

```cpp
void setBit(int index, bool value)
```

Sets or clears a single bit within the stored byte. 
#### Parameters
* `index` Bit position (0 = LSB, 7 = MSB). 

* `value` true to set, false to clear.

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `int` |  |
| `value` | `bool` |  |

---

{#read-12}

#### read

`virtual`

```cpp
virtual void read(BitReader & reader)
```

Reads the body (not the type or size) for this type of attribute from the given buffer. 
#### Parameters
* `reader` Source bit reader positioned at the attribute body.

| Parameter | Type | Description |
|-----------|------|-------------|
| `reader` | `[BitReader](base.md#bitreader) &` |  |

---

{#write-27}

#### write

`virtual` `const`

```cpp
virtual void write(BitWriter & writer) const
```

Writes the body (not the type or size) to the given buffer. 
#### Parameters
* `writer` Destination bit writer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `writer` | `[BitWriter](base.md#bitwriter) &` |  |

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `constexpr uint16_t` | [`Size`](#size-10) `static` |  |

---

{#size-10}

#### Size

`static`

```cpp
constexpr uint16_t Size = 1
```

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint8_t` | [`_bits`](#_bits-2)  |  |

---

{#_bits-2}

#### _bits

```cpp
uint8_t _bits
```

