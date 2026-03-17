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


#include "scy/base.h"
#include "scy/buffer.h"
#include "scy/packetsignal.h"
#include "scy/util.h"

#include <stdexcept>


namespace scy {


class /* Base_API */ IPacketCreationStrategy
{
public:
    IPacketCreationStrategy() {}
    virtual ~IPacketCreationStrategy() = default;
    virtual IPacket* create(const ConstBuffer& buffer, size_t& nread) const = 0;

    virtual int priority() const = 0; // 0 - 100

    static bool compareProiroty(const IPacketCreationStrategy* l,
                                const IPacketCreationStrategy* r)
    {
        return l->priority() > r->priority();
    }
};


using PacketCreationStrategyList = std::vector<IPacketCreationStrategy*>;


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

    virtual ~PacketCreationStrategy()
    {
    }

    virtual IPacket* create(const ConstBuffer& buffer, size_t& nread) const override
    {
        auto packet = new PacketT;
        if ((nread = packet->read(buffer)) > 0)
            return packet;
        delete packet;
        return nullptr;
    }

    virtual int priority() const override
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
    PacketFactory()
    {
    }

    virtual ~PacketFactory()
    {
        util::clearVector(_types);
    }

    template <class PacketT>
    void registerPacketType(int priority)
    {
        unregisterPacketType<PacketT>(); // ensure unique values

        _types.push_back(new PacketCreationStrategy<PacketT>(priority));
        sort(_types.begin(), _types.end(),
             IPacketCreationStrategy::compareProiroty);
    }

    template <class PacketT>
    void unregisterPacketType()
    {

        for (auto it = _types.begin(); it != _types.end(); ++it) {
            if (dynamic_cast<PacketCreationStrategy<PacketT>*>(*it) != nullptr) {
                delete *it;
                _types.erase(it);
                return;
            }
        }
    }

    template <class StrategyT>
    void registerStrategy(int priority)
    {
        unregisterStrategy<StrategyT>(); // ensure unique values

        _types.push_back(new StrategyT(priority));
        std::sort(_types.begin(), _types.end(),
                  IPacketCreationStrategy::compareProiroty);
    }

    template <class StrategyT>
    void unregisterStrategy()
    {

        for (auto it = _types.begin(); it != _types.end(); ++it) {
            if (dynamic_cast<StrategyT*>(*it) != nullptr) {
                delete *it;
                _types.erase(it);
                return;
            }
        }
    }

    PacketCreationStrategyList& types()
    {
        return _types;
    }

    PacketCreationStrategyList types() const
    {
        return _types;
    }

    virtual bool onPacketCreated(IPacket*)
    {
        // returning false will stop packet propagation
        return true;
    }

    virtual IPacket* createPacket(const ConstBuffer& buffer, size_t& nread)
    {
        // size_t offset = reader.position();
        if (_types.empty())
            throw std::logic_error("No packet types registered");

        for (unsigned i = 0; i < _types.size(); i++) {
            auto packet = _types[i]->create(buffer, nread);
            if (packet) {
                if (!onPacketCreated(packet)) {
                    delete packet;
                    continue;
                }
                return packet;
            }
            // reader.seek(offset);
        }
        return nullptr;
    }

protected:
    PacketCreationStrategyList _types;
};


} // namespace scy


/// @\}
