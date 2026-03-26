///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/collection.h"
#include "icy/packetstream.h"


namespace icy {


/// Manages a named collection of PacketStream instances with lifecycle callbacks
class /* ICY_EXTERN */ StreamManager : public KeyedStore<std::string, PacketStream>
{
public:
    StreamManager();
    ~StreamManager();

    bool addStream(PacketStream* stream, bool whiny = true);
    bool addStream(std::unique_ptr<PacketStream> stream, bool whiny = true);
    bool closeStream(const std::string& name, bool whiny = true);
    void closeAll();

    PacketStream* getStream(const std::string& name, bool whiny = true) const;
    PacketStream* getDefaultStream() const;

    void print(std::ostream& os) const;

protected:
    void onAdd(const std::string&, PacketStream* stream) override;
    void onRemove(const std::string&, PacketStream* stream) override;

    void onStreamStateChange(void* sender, PacketStreamState& state, const PacketStreamState&);

    virtual const char* className() const { return "Stream Manager"; };
};


} // namespace icy


/// @}
