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


/// Abstract strategy for creating typed packets from raw buffer data
class /* Base_API */ IPacketCreationStrategy
{
public:
    IPacketCreationStrategy() = default;
    virtual ~IPacketCreationStrategy() = default;
    /// Attempts to create a typed packet from the given buffer.
    /// @param buffer Raw input data.
    /// @param nread Set to the number of bytes consumed on success, 0 otherwise.
    /// @return Newly allocated packet on success, nullptr if the buffer does not match.
    virtual IPacket* create(const ConstBuffer& buffer, size_t& nread) const = 0;

    /// Returns the dispatch priority of this strategy (0–100; higher runs first).
    virtual int priority() const = 0; // 0 - 100
};


using PacketCreationStrategyPtr = std::unique_ptr<IPacketCreationStrategy>;
using PacketCreationStrategyList = std::vector<PacketCreationStrategyPtr>;


/// This template class implements an adapter that sits between
/// an SignalBase and an object receiving notifications from it.
template <class PacketT>
struct PacketCreationStrategy : public IPacketCreationStrategy
{
    /// @param priority Dispatch priority in the range 0–100.
    /// @throws std::logic_error if priority exceeds 100.
    PacketCreationStrategy(int priority = 0)
        : _priority(priority)
    {
        if (_priority > 100)
            throw std::logic_error("PacketCreationStrategy priority must be <= 100");
    }

    /// Attempts to default-construct a `PacketT`, calling its `read()` method.
    /// @param buffer Raw input data.
    /// @param nread Set to the number of bytes consumed when read() succeeds.
    /// @return Newly allocated `PacketT` on success, nullptr if read() returns 0.
    IPacket* create(const ConstBuffer& buffer, size_t& nread) const override
    {
        auto packet = std::make_unique<PacketT>();
        if ((nread = packet->read(buffer)) > 0)
            return packet.release();
        return nullptr;
    }

    /// @return The priority value assigned at construction.
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


/// Priority-ordered factory that creates typed packets from raw buffers using registered strategies
class /* Base_API */ PacketFactory
{
public:
    PacketFactory() = default;
    virtual ~PacketFactory() = default;

    /// Registers a `PacketCreationStrategy<PacketT>` at the given priority.
    /// Any previously registered strategy for `PacketT` is replaced.
    /// @tparam PacketT Packet type to register; must default-constructible with a `read()` method.
    /// @param priority Dispatch priority (0–100; higher runs first).
    template <class PacketT>
    void registerPacketType(int priority)
    {
        unregisterPacketType<PacketT>();
        _types.push_back(std::make_unique<PacketCreationStrategy<PacketT>>(priority));
        sortTypes();
    }

    /// Removes the `PacketCreationStrategy<PacketT>` from the factory, if present.
    /// @tparam PacketT Packet type whose strategy should be removed.
    template <class PacketT>
    void unregisterPacketType()
    {
        auto it = std::find_if(_types.begin(), _types.end(), [](const auto& s) {
            return dynamic_cast<PacketCreationStrategy<PacketT>*>(s.get()) != nullptr;
        });
        if (it != _types.end())
            _types.erase(it);
    }

    /// Registers an arbitrary `IPacketCreationStrategy` subclass at the given priority.
    /// Any previously registered instance of the same type is replaced.
    /// @tparam StrategyT Strategy type to instantiate; must accept a priority int in its constructor.
    /// @param priority Dispatch priority (0–100; higher runs first).
    template <class StrategyT>
    void registerStrategy(int priority)
    {
        unregisterStrategy<StrategyT>();
        _types.push_back(std::make_unique<StrategyT>(priority));
        sortTypes();
    }

    /// Removes the `StrategyT` instance from the factory, if present.
    /// @tparam StrategyT Strategy type to remove.
    template <class StrategyT>
    void unregisterStrategy()
    {
        auto it = std::find_if(_types.begin(), _types.end(), [](const auto& s) {
            return dynamic_cast<StrategyT*>(s.get()) != nullptr;
        });
        if (it != _types.end())
            _types.erase(it);
    }

    /// @return Mutable reference to the ordered list of registered strategies.
    PacketCreationStrategyList& types()
    {
        return _types;
    }

    /// @return Const reference to the ordered list of registered strategies.
    const PacketCreationStrategyList& types() const
    {
        return _types;
    }

    /// Called after a packet is successfully created by a strategy.
    /// Override to apply filtering; return false to reject the packet (it will be deleted).
    /// @param packet Newly created packet.
    /// @return True to accept the packet, false to discard it and try the next strategy.
    virtual bool onPacketCreated(IPacket* packet)
    {
        (void)packet;
        return true;
    }

    /// Iterates registered strategies in priority order and returns the first
    /// successfully created packet.
    /// @param buffer Raw input data.
    /// @param nread Set to the number of bytes consumed on success.
    /// @return Newly allocated packet, or nullptr if no strategy matched.
    /// @throws std::logic_error if no packet types have been registered.
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


/// @}
