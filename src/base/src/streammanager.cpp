///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/streammanager.h"
#include "scy/logger.h"


using namespace std;


namespace scy {


StreamManager::StreamManager()
{
}


StreamManager::~StreamManager()
{
    closeAll();
}


void StreamManager::closeAll()
{
    std::unique_lock<std::shared_mutex> guard(_mutex);

    LDebug("Close all streams: ", _map.size());
    for (auto it = _map.begin(); it != _map.end();) {
        it->second->StateChange -= slot(this, &StreamManager::onStreamStateChange);
        it->second->close();
        it = _map.erase(it); // unique_ptr deletes the stream
    }
}


bool StreamManager::addStream(PacketStream* stream, bool whiny)
{
    if (!stream)
        throw std::invalid_argument("StreamManager: stream cannot be null");
    if (stream->name().empty())
        throw std::invalid_argument("StreamManager: stream name cannot be empty");
    return Manager::add(stream->name(), stream, whiny);
}


PacketStream* StreamManager::getStream(const std::string& name, bool whiny)
{
    return Manager::get(name, whiny);
}


bool StreamManager::closeStream(const std::string& name, bool whiny)
{
    if (name.empty())
        throw std::invalid_argument("StreamManager: stream name cannot be empty");

    LDebug("Close stream: ", name);
    PacketStream* stream = get(name, whiny);
    if (stream) {
        stream->close();
        return true;
    }
    return false;
}


PacketStream* StreamManager::getDefaultStream()
{
    std::shared_lock<std::shared_mutex> guard(_mutex);

    // Returns the first stream or nullptr.
    if (!_map.empty()) {
        return _map.begin()->second.get();
    }

    return nullptr;
}


void StreamManager::onAdd(const std::string&, PacketStream* stream)
{
    // Stream name can't be empty
    if (stream->name().empty())
        throw std::invalid_argument("StreamManager: stream name cannot be empty");

    // Receive callbacks after all other listeners
    // so we can delete the stream when it closes.
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

    // Cantch stream closed state and free it if necessary
    if (state.equals(PacketStreamState::Closed)) {
        auto stream = reinterpret_cast<PacketStream*>(sender);
        stream->StateChange -= slot(this, &StreamManager::onStreamStateChange);
        LDebug("On stream close: freeing: ", stream->name());
        bool success = Manager::free(stream->name());
        if (!success) {
            LWarn("Cannot remove stream: ", stream->name());
        }
    }
}


const StreamManager::Map& StreamManager::streams() const
{
    std::shared_lock<std::shared_mutex> guard(_mutex);
    return _map;
}


void StreamManager::print(std::ostream& os) const
{
    std::shared_lock<std::shared_mutex> guard(_mutex);
    os << "StreamManager[";
    for (const auto& [name, stream] : _map) {
        os << "\n\t" << stream.get() << ": " << name;
    }
    os << "\n]";
}


} // namespace scy
