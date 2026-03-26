///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#pragma once


#include "icy/packet.h"
#include "icy/stun/attributes.h"
#include "icy/stun/stun.h"

#include <memory>
#include <vector>


namespace icy {
namespace stun {


using TransactionID = std::string; ///< Fixed-width 12-byte STUN transaction identifier stored as raw bytes.


/// STUN/TURN protocol message with method, class, transaction ID, and attributes
class STUN_API Message : public IPacket
{
public:
    enum MethodType
    {
        Undefined = 0x0000, ///< default error type

        /// STUN
        Binding = 0x0001,

        /// TURN
        Allocate = 0x0003, ///< (only request/response semantics defined)
        Refresh = 0x0004,
        SendIndication = 0x0006,   ///< (only indication semantics defined)
        DataIndication = 0x0007,   ///< (only indication semantics defined)
        CreatePermission = 0x0008, ///< (only request/response semantics defined)
        ChannelBind = 0x0009,      ///< (only request/response semantics defined)

        /// TURN TCP RFC 6062
        Connect = 0x000a,
        ConnectionBind = 0x000b,
        ConnectionAttempt = 0x000c
    };

    enum ClassType
    {
        Request = 0x0000,
        Indication = 0x0010,
        SuccessResponse = 0x0100,
        ErrorResponse = 0x0110
    };

    enum ErrorCodes
    {
        TryAlternate = 300,
        BadRequest = 400,
        NotAuthorized = 401,
        Forbidden = 403,
        UnknownAttribute = 420,
        StaleCredentials = 430,
        IntegrityCheckFailure = 431,
        MissingUsername = 432,
        UseTLS = 433,
        AllocationMismatch = 437,
        StaleNonce = 438,
        WrongCredentials = 441,
        UnsupportedTransport = 442,
        AllocationQuotaReached = 486,
        RoleConflict = 487,
        ServerError = 500,
        InsufficientCapacity = 508,
        GlobalFailure = 600,

        /// TURN TCP
        ConnectionAlreadyExists = 446,
        ConnectionTimeoutOrFailure = 447
    };

public:
    /// Constructs a default message (Request class, Undefined method) with a
    /// randomly generated 12-byte transaction ID.
    Message();

    /// Constructs a message with explicit class and method.
    /// @param clss  Message class (Request, Indication, SuccessResponse, or ErrorResponse).
    /// @param meth  Message method (Binding, Allocate, Refresh, etc.).
    Message(ClassType clss, MethodType meth);

    /// Deep-copy constructor; clones all attributes.
    Message(const Message& that);

    /// Move constructor.
    Message(Message&& that) noexcept;

    /// Deep-copy assignment; clones all attributes from @p that.
    Message& operator=(const Message& that);

    /// Move assignment.
    Message& operator=(Message&& that) noexcept;

    virtual ~Message();

    /// @return A heap-allocated deep copy of this message.
    std::unique_ptr<IPacket> clone() const override;

    /// Sets the message class field.
    /// @param type One of Request, Indication, SuccessResponse, ErrorResponse.
    void setClass(ClassType type);

    /// Sets the message method field.
    /// @param type One of the MethodType enumerators.
    void setMethod(MethodType type);

    /// Sets the 12-byte transaction ID.
    /// @param id Must be exactly kTransactionIdLength (12) bytes.
    void setTransactionID(const std::string& id);

    /// @return The message class.
    [[nodiscard]] ClassType classType() const;

    /// @return The message method.
    [[nodiscard]] MethodType methodType() const;

    /// @return Reference to the 12-byte transaction ID string.
    [[nodiscard]] const TransactionID& transactionID() const { return _transactionID; }

    /// @return Total body size in bytes (sum of padded attribute headers and bodies).
    [[nodiscard]] size_t size() const { return static_cast<size_t>(computeBodySize()); }

    /// @return Human-readable method name (e.g. "BINDING", "ALLOCATE").
    [[nodiscard]] std::string methodString() const;

    /// @return Human-readable class name (e.g. "Request", "SuccessResponse").
    [[nodiscard]] std::string classString() const;

    /// Maps a numeric error code to its canonical string description.
    /// @param errorCode One of the ErrorCodes enumerators.
    /// @return Human-readable error string, or "UnknownError" if not recognised.
    [[nodiscard]] std::string errorString(uint16_t errorCode) const;

    /// Constructs an attribute of type T in-place and appends it to the message.
    /// Returns a reference to the new attribute for further configuration.
    /// @tparam T Concrete attribute type (e.g. stun::Lifetime, stun::XorMappedAddress).
    /// @return Reference to the newly added attribute.
    template <typename T>
    T& add()
    {
        auto attr = std::make_unique<T>();
        auto& ref = *attr;
        add(std::move(attr));
        return ref;
    }

    /// Appends an attribute to the message, taking ownership via unique_ptr.
    /// @param attr Attribute to add.
    void add(std::unique_ptr<Attribute> attr);

    /// Returns the Nth attribute of the given type, or nullptr if not found.
    /// @param type  Attribute type code to search for.
    /// @param index Zero-based occurrence index (0 = first match).
    /// @return Raw pointer to the attribute (owned by this message), or nullptr.
    [[nodiscard]] Attribute* get(Attribute::Type type, int index = 0) const;

    /// Type-safe attribute accessor using the concrete attribute's TypeID.
    /// @tparam T    Concrete attribute type (must define TypeID).
    /// @param index Zero-based occurrence index.
    /// @return Pointer to T, or nullptr if the attribute is absent.
    template <typename T>
    [[nodiscard]] T* get(int index = 0) const
    {
        return reinterpret_cast<T*>(
            get(static_cast<Attribute::Type>(T::TypeID), index));
    }

    /// Parses a STUN/TURN packet from the given buffer.
    /// @param buf Buffer containing at least one complete STUN message.
    /// @return Number of bytes consumed, or 0 on parse failure.
    ssize_t read(const ConstBuffer& buf);

    /// Serialises this message into a STUN/TURN wire-format packet.
    /// @param buf Destination buffer; data is appended.
    void write(Buffer& buf) const;

    /// @return A concise string representation for logging (method, transaction ID, attribute types).
    [[nodiscard]] std::string toString() const;

    /// Writes the same representation as toString() to the given stream.
    /// @param os Output stream.
    void print(std::ostream& os) const;

    virtual const char* className() const { return "StunMessage"; }

protected:
    uint16_t _class;
    uint16_t _method;
    uint16_t _size; ///< Set by read(); write() uses computeBodySize() instead
    TransactionID _transactionID;
    std::vector<std::unique_ptr<Attribute>> _attrs;

private:
    /// Computes the wire body size from the current attribute list.
    [[nodiscard]] uint16_t computeBodySize() const;
};


/// Returns true if @p methodType corresponds to a recognised STUN/TURN method.
/// Used during parsing to reject malformed packets.
/// @param methodType Raw method bits extracted from the message type field.
/// @return true if the method is one of the defined MethodType values.
[[nodiscard]] constexpr bool isValidMethod(uint16_t methodType)
{
    switch (methodType) {
        case Message::Binding:
        case Message::Allocate:
        case Message::Refresh:
        case Message::SendIndication:
        case Message::DataIndication:
        case Message::CreatePermission:
        case Message::ChannelBind:
        case Message::Connect:
        case Message::ConnectionBind:
        case Message::ConnectionAttempt:
            return true;
    }
    return false;
}
} // namespace stun
} // namespace icy


/// @}
