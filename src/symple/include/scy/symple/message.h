///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/packet.h"
#include "scy/symple/address.h"
#include "scy/symple/peer.h"
#include "scy/symple/symple.h"


namespace scy {
namespace smpl {


class Symple_API Message : public json::value
    , public IPacket
{
public:
    Message();
    Message(const json::value& root);
    Message(const Message& root);
    virtual ~Message();

    virtual std::unique_ptr<IPacket> clone() const;

    virtual bool valid() const;
    virtual void clear();
    virtual void clearData();
    virtual void clearNotes();

    [[nodiscard]] std::string type() const;
    [[nodiscard]] std::string id() const;
    [[nodiscard]] Address to() const;
    [[nodiscard]] Address from() const;
    [[nodiscard]] int status() const;

    void setType(const std::string& type);
    void setTo(const Peer& to);
    void setTo(const Address& to);
    void setTo(const std::string& to);
    void setFrom(const Peer& from);
    void setFrom(const Address& from);
    void setFrom(const std::string& from);

    /// HTTP status codes are used to describe the message response.
    /// @see http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
    void setStatus(int code);

    json::value& notes();

    /// Set a note for the application
    /// Possible "type" values: `info`, `warn`, `error`
    void setNote(const std::string& type, const std::string& text);
    void addNote(const std::string& type, const std::string& text);

    json::value data(const std::string& name) const;
    json::value& data(const std::string& name);
    json::value& setData(const std::string& name);
    void setData(const std::string& name, const char* data);
    void setData(const std::string& name, const std::string& data);
    void setData(const std::string& name, const json::value& data);
    void setData(const std::string& name, int data);
    void removeData(const std::string& name);
    bool hasData(const std::string& name);

    virtual ssize_t read(const ConstBuffer& buf);
    virtual ssize_t read(const std::string& root);
    virtual void write(Buffer& buf) const;

    [[nodiscard]] bool isRequest() const;
    virtual size_t size() const;

    void print(std::ostream& os) const;

    virtual const char* className() const { return "Symple::Message"; }
};


} // namespace smpl
} // namespace scy


/// @\}
