///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/streammanager.h"
#include "icy/logger.h"


namespace icy {


StreamManager::StreamManager()
{
}


StreamManager::~StreamManager()
{
    closeAll();
}


bool StreamManager::addStream(PacketStream* stream, bool whiny)
{
    return addStream(std::unique_ptr<PacketStream>(stream), whiny);
}


bool StreamManager::addStream(std::unique_ptr<PacketStream> stream, bool whiny)
{
    if (!stream)
        throw std::invalid_argument("StreamManager: stream cannot be null");
    if (stream->name().empty())
        throw std::invalid_argument("StreamManager: stream name cannot be empty");

    if (contains(stream->name())) {
        if (whiny)
            throw std::runtime_error("StreamManager: stream already exists: " + stream->name());
        return false;
    }

    add(stream->name(), std::move(stream));
    return true;
}


PacketStream* StreamManager::getStream(const std::string& name, bool whiny) const
{
    auto* stream = get(name);
    if (!stream && whiny)
        throw std::runtime_error("StreamManager: stream not found: " + name);
    return stream;
}


bool StreamManager::closeStream(const std::string& name, bool whiny)
{
    if (name.empty())
        throw std::invalid_argument("StreamManager: stream name cannot be empty");

    LDebug("Close stream: ", name);
    PacketStream* stream = getStream(name, whiny);
    if (stream) {
        stream->close();
        return true;
    }
    return false;
}


void StreamManager::closeAll()
{
    LDebug("Close all streams: ", size());
    for (auto& [name, stream] : _map) {
        stream->StateChange -= slot(this, &StreamManager::onStreamStateChange);
        stream->close();
    }
    _map.clear();
}


PacketStream* StreamManager::getDefaultStream() const
{
    if (!empty())
        return _map.begin()->second.get();
    return nullptr;
}


void StreamManager::onAdd(const std::string&, PacketStream* stream)
{
    LDebug("stream added: ", stream->name());
    stream->StateChange += slot(this, &StreamManager::onStreamStateChange, -1);
}


void StreamManager::onRemove(const std::string&, PacketStream* stream)
{
    LDebug("stream removed: ", stream->name());
    stream->StateChange -= slot(this, &StreamManager::onStreamStateChange);
}


void StreamManager::onStreamStateChange(void* sender, PacketStreamState& state,
                                        const PacketStreamState&)
{
    LDebug("Stream state change: ", state);

    if (state.equals(PacketStreamState::Closed)) {
        auto stream = reinterpret_cast<PacketStream*>(sender);
        stream->StateChange -= slot(this, &StreamManager::onStreamStateChange);
        LDebug("On stream close: freeing: ", stream->name());
        erase(stream->name());
    }
}


void StreamManager::print(std::ostream& os) const
{
    os << "StreamManager[";
    for (const auto& [name, stream] : _map) {
        os << "\n\t" << stream.get() << ": " << name;
    }
    os << "\n]";
}


} // namespace icy
