///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
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


using TransactionID = std::string;


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
        BadRequest = 400,
        NotAuthorized = 401,
        UnknownAttribute = 420,
        StaleCredentials = 430,
        IntegrityCheckFailure = 431,
        MissingUsername = 432,
        UseTLS = 433,
        RoleConflict = 487,
        ServerError = 500,
        GlobalFailure = 600,

        /// TURN TCP
        ConnectionAlreadyExists = 446,
        ConnectionTimeoutOrFailure = 447
    };

public:
    Message();
    Message(ClassType clss, MethodType meth);
    Message(const Message& that);
    Message(Message&& that) noexcept;
    Message& operator=(const Message& that);
    Message& operator=(Message&& that) noexcept;
    virtual ~Message();

    std::unique_ptr<IPacket> clone() const override;

    void setClass(ClassType type);
    void setMethod(MethodType type);
    void setTransactionID(const std::string& id);

    [[nodiscard]] ClassType classType() const;
    [[nodiscard]] MethodType methodType() const;
    [[nodiscard]] const TransactionID& transactionID() const { return _transactionID; }
    [[nodiscard]] size_t size() const { return static_cast<size_t>(_size); }

    [[nodiscard]] std::string methodString() const;
    [[nodiscard]] std::string classString() const;
    [[nodiscard]] std::string errorString(uint16_t errorCode) const;

    /// Takes ownership of the raw pointer (wraps in unique_ptr).
    void add(Attribute* attr);

    /// Takes ownership via unique_ptr.
    void add(std::unique_ptr<Attribute> attr);

    [[nodiscard]] Attribute* get(Attribute::Type type, int index = 0) const;

    template <typename T>
    [[nodiscard]] T* get(int index = 0) const
    {
        return reinterpret_cast<T*>(
            get(static_cast<Attribute::Type>(T::TypeID), index));
    }

    /// Parses the STUN/TURN packet from the given buffer.
    /// The return value indicates the number of bytes read.
    ssize_t read(const ConstBuffer& buf);

    /// Writes this object into a STUN/TURN packet.
    void write(Buffer& buf) const;

    [[nodiscard]] std::string toString() const;
    void print(std::ostream& os) const;

    virtual const char* className() const { return "StunMessage"; }

protected:
    uint16_t _class;
    uint16_t _method;
    uint16_t _size;
    TransactionID _transactionID;
    std::vector<std::unique_ptr<Attribute>> _attrs;
};


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


/// @\}
