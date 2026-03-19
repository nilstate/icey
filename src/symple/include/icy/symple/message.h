///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/packet.h"
#include "icy/symple/address.h"
#include "icy/symple/peer.h"
#include "icy/symple/symple.h"
#include <string_view>


namespace icy {
namespace smpl {


class Symple_API Message : public json::Value
    , public IPacket
{
public:
    using IPacket::data; // inherit IPacket::data()

    Message();
    Message(const json::Value& root);
    Message(const Message& root);
    Message& operator=(const Message&) = default;
    virtual ~Message();

    std::unique_ptr<IPacket> clone() const override;

    virtual bool valid() const;
    virtual void clear();
    virtual void clearData();
    virtual void clearNotes();

    [[nodiscard]] std::string type() const;
    [[nodiscard]] std::string id() const;
    [[nodiscard]] Address to() const;
    [[nodiscard]] Address from() const;
    [[nodiscard]] int status() const;

    void setType(std::string_view type);
    void setTo(const Peer& to);
    void setTo(const Address& to);
    void setTo(std::string_view to);
    void setFrom(const Peer& from);
    void setFrom(const Address& from);
    void setFrom(std::string_view from);

    /// HTTP status codes are used to describe the message response.
    /// @see http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
    void setStatus(int code);

    json::Value& notes();

    /// Set a note for the application
    /// Possible "type" values: `info`, `warn`, `error`
    void setNote(std::string_view type, std::string_view text);
    void addNote(std::string_view type, std::string_view text);

    json::Value data(std::string_view name) const;
    json::Value& data(std::string_view name);
    json::Value& setData(std::string_view name);
    void setData(std::string_view name, const char* data);
    void setData(std::string_view name, std::string_view data);
    void setData(std::string_view name, const json::Value& data);
    void setData(std::string_view name, int data);
    void removeData(std::string_view name);
    bool hasData(std::string_view name);

    ssize_t read(const ConstBuffer& buf) override;
    virtual ssize_t read(const std::string& root);
    void write(Buffer& buf) const override;

    [[nodiscard]] bool isRequest() const;
    size_t size() const override;

    void print(std::ostream& os) const;

    virtual const char* className() const { return "Symple::Message"; }
};


} // namespace smpl
} // namespace icy


/// @\}
