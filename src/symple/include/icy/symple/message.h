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


/// Base Symple protocol message with addressing, status, data, and notes
class Symple_API Message : public json::Value
    , public IPacket
{
public:
    using IPacket::data; // inherit IPacket::data()

    /// Constructs a new message with a random ID and type set to "message".
    Message();

    /// Constructs a message from a JSON value; sets missing id/type fields.
    /// @param root JSON object to initialise from.
    Message(const json::Value& root);

    /// Copy constructor; preserves or generates id/type fields.
    /// @param root Source message.
    Message(const Message& root);

    Message& operator=(const Message&) = default;
    virtual ~Message();

    /// Returns a heap-allocated copy of this message.
    std::unique_ptr<IPacket> clone() const override;

    /// Returns true if the message has both `type` and `id` fields.
    virtual bool valid() const;

    /// Clears all JSON fields from this message.
    virtual void clear();

    /// Clears the `data` sub-object.
    virtual void clearData();

    /// Clears the `notes` array.
    virtual void clearNotes();

    /// Returns the message type string (defaults to "message").
    [[nodiscard]] std::string type() const;

    /// Returns the message ID string.
    [[nodiscard]] std::string id() const;

    /// Returns the recipient address parsed from the `to` field.
    [[nodiscard]] Address to() const;

    /// Returns the sender address parsed from the `from` field.
    [[nodiscard]] Address from() const;

    /// Returns the user component of the `to` address without constructing an Address.
    [[nodiscard]] std::string toUser() const;

    /// Returns the id component of the `to` address without constructing an Address.
    [[nodiscard]] std::string toId() const;

    /// Returns the user component of the `from` address without constructing an Address.
    [[nodiscard]] std::string fromUser() const;

    /// Returns the id component of the `from` address without constructing an Address.
    [[nodiscard]] std::string fromId() const;

    /// Returns the HTTP status code, or -1 if not set.
    [[nodiscard]] int status() const;

    /// Sets the message type field.
    /// @param type Type string (e.g. "message", "command").
    void setType(std::string_view type);

    /// Sets the `to` field from a peer's address.
    /// @param to Destination peer.
    void setTo(const Peer& to);

    /// Sets the `to` field from an address object.
    /// @param to Destination address.
    void setTo(const Address& to);

    /// Sets the `to` field from an address string.
    /// @param to Destination address string.
    void setTo(std::string_view to);

    /// Sets the `from` field from a peer's address.
    /// @param from Sender peer.
    void setFrom(const Peer& from);

    /// Sets the `from` field from an address object.
    /// @param from Sender address.
    void setFrom(const Address& from);

    /// Sets the `from` field from an address string.
    /// @param from Sender address string.
    void setFrom(std::string_view from);

    /// HTTP status codes are used to describe the message response.
    /// Throws std::invalid_argument if code is outside [101, 504].
    /// @param code HTTP status code.
    /// @see http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
    void setStatus(int code);

    /// Returns a reference to the `notes` JSON array.
    json::Value& notes();

    /// Replaces all notes with a single note.
    /// @param type Note severity: `info`, `warn`, or `error`.
    /// @param text Note message text.
    void setNote(std::string_view type, std::string_view text);

    /// Appends a note to the `notes` array.
    /// @param type Note severity: `info`, `warn`, or `error`.
    /// @param text Note message text.
    void addNote(std::string_view type, std::string_view text);

    /// Returns a copy of the named data field.
    /// @param name Field name within `data`.
    json::Value data(std::string_view name) const;

    /// Returns a reference to the named data field (creates it if absent).
    /// @param name Field name within `data`.
    json::Value& data(std::string_view name);

    /// Creates or replaces a named data field; returns a reference to it.
    /// @param name Field name within `data`.
    json::Value& setData(std::string_view name);

    /// Sets a named data field to a C-string value.
    /// @param name Field name within `data`.
    /// @param data String value to assign.
    void setData(std::string_view name, const char* data);

    /// Sets a named data field to a string value.
    /// @param name Field name within `data`.
    /// @param data String value to assign.
    void setData(std::string_view name, std::string_view data);

    /// Sets a named data field to a JSON value.
    /// @param name Field name within `data`.
    /// @param data JSON value to assign.
    void setData(std::string_view name, const json::Value& data);

    /// Sets a named data field to an integer value.
    /// @param name Field name within `data`.
    /// @param data Integer value to assign.
    void setData(std::string_view name, int data);

    /// Removes a named field from the `data` sub-object.
    /// @param name Field name to remove.
    void removeData(std::string_view name);

    /// Returns true if the named field exists in the `data` sub-object.
    /// @param name Field name to look up.
    bool hasData(std::string_view name);

    /// Deserialises the message from a raw buffer.
    /// @param buf Buffer containing the JSON payload.
    /// @return Number of bytes consumed.
    ssize_t read(const ConstBuffer& buf) override;

    /// Deserialises the message from a JSON string.
    /// @param root JSON string to parse.
    /// @return Number of characters consumed.
    virtual ssize_t read(const std::string& root);

    /// Serialises the message as JSON into a buffer.
    /// @param buf Buffer to append to.
    void write(Buffer& buf) const override;

    /// Returns true if no status code has been set (i.e. status() == -1).
    [[nodiscard]] bool isRequest() const;

    /// Returns the serialised JSON size in bytes.
    size_t size() const override;

    /// Pretty-prints the message JSON to the given stream.
    /// @param os Output stream.
    void print(std::ostream& os) const;

    virtual const char* className() const { return "Symple::Message"; }
};


} // namespace smpl
} // namespace icy


/// @}
