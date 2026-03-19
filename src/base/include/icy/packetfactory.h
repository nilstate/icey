///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/buffer.h"
#include "icy/packetsignal.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>


namespace icy {


class /* Base_API */ IPacketCreationStrategy
{
public:
    IPacketCreationStrategy() = default;
    virtual ~IPacketCreationStrategy() = default;
    virtual IPacket* create(const ConstBuffer& buffer, size_t& nread) const = 0;

    virtual int priority() const = 0; // 0 - 100
};


using PacketCreationStrategyPtr = std::unique_ptr<IPacketCreationStrategy>;
using PacketCreationStrategyList = std::vector<PacketCreationStrategyPtr>;


/// This template class implements an adapter that sits between
/// an SignalBase and an object receiving notifications from it.
template <class PacketT>
struct PacketCreationStrategy : public IPacketCreationStrategy
{
    PacketCreationStrategy(int priority = 0)
        : _priority(priority)
    {
        if (_priority > 100)
            throw std::logic_error("PacketCreationStrategy priority must be <= 100");
    }

    IPacket* create(const ConstBuffer& buffer, size_t& nread) const override
    {
        auto packet = std::make_unique<PacketT>();
        if ((nread = packet->read(buffer)) > 0)
            return packet.release();
        return nullptr;
    }

    int priority() const override
    {
        return _priority;
    }

protected:
    int _priority;
};


//
// Packet Factory
//


class /* Base_API */ PacketFactory
{
public:
    PacketFactory() = default;
    virtual ~PacketFactory() = default;

    template <class PacketT>
    void registerPacketType(int priority)
    {
        unregisterPacketType<PacketT>();
        _types.push_back(std::make_unique<PacketCreationStrategy<PacketT>>(priority));
        sortTypes();
    }

    template <class PacketT>
    void unregisterPacketType()
    {
        auto it = std::find_if(_types.begin(), _types.end(), [](const auto& s) {
            return dynamic_cast<PacketCreationStrategy<PacketT>*>(s.get()) != nullptr;
        });
        if (it != _types.end())
            _types.erase(it);
    }

    template <class StrategyT>
    void registerStrategy(int priority)
    {
        unregisterStrategy<StrategyT>();
        _types.push_back(std::make_unique<StrategyT>(priority));
        sortTypes();
    }

    template <class StrategyT>
    void unregisterStrategy()
    {
        auto it = std::find_if(_types.begin(), _types.end(), [](const auto& s) {
            return dynamic_cast<StrategyT*>(s.get()) != nullptr;
        });
        if (it != _types.end())
            _types.erase(it);
    }

    PacketCreationStrategyList& types()
    {
        return _types;
    }

    const PacketCreationStrategyList& types() const
    {
        return _types;
    }

    virtual bool onPacketCreated(IPacket*)
    {
        return true;
    }

    virtual IPacket* createPacket(const ConstBuffer& buffer, size_t& nread)
    {
        if (_types.empty())
            throw std::logic_error("No packet types registered");

        for (auto& strategy : _types) {
            auto packet = strategy->create(buffer, nread);
            if (packet) {
                if (!onPacketCreated(packet)) {
                    delete packet;
                    continue;
                }
                return packet;
            }
        }
        return nullptr;
    }

protected:
    PacketCreationStrategyList _types;

private:
    void sortTypes()
    {
        std::sort(_types.begin(), _types.end(),
                  [](const auto& l, const auto& r) {
                      return l->priority() > r->priority();
                  });
    }
};


} // namespace icy


/// @\}
