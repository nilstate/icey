///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/collection.h"
#include "scy/packetstream.h"


namespace scy {


using StreamManagerBase = LiveCollection<std::string, PacketStream>;


class /* SCY_EXTERN */ StreamManager : public StreamManagerBase
{
public:
    using Manager = StreamManagerBase;
    using Map = Manager::Map;

public:
    StreamManager();
    virtual ~StreamManager();

    virtual bool addStream(PacketStream* stream, bool whiny = true);
    virtual bool closeStream(const std::string& name, bool whiny = true);
    virtual void closeAll();
    virtual PacketStream* getStream(const std::string& name, bool whiny = true);

    /// Returns the first stream in the list, or nullptr.
    virtual PacketStream* getDefaultStream();

    virtual const Map& streams() const;

    virtual void print(std::ostream& os) const;

protected:
    /// Called after a stream is added.
    virtual void onAdd(const std::string&, PacketStream* task) override;

    /// Called after a stream is removed.
    virtual void onRemove(const std::string&, PacketStream* task) override;

    virtual void onStreamStateChange(void* sender, PacketStreamState& state, const PacketStreamState&);

    virtual const char* className() const { return "Stream Manager"; };

protected:
};


} // namespace scy


/// @\}
