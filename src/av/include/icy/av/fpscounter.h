///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "icy/av/packet.h"
#include "icy/packetstream.h"

#include <time.h>


namespace icy {
namespace av {


/// FPS counter based on the simple moving average (SMA) algorithm
class AV_API FPSCounter
{
private:
    static const int MAX = 100;

    int tickIndex;
    clock_t prevTick;
    clock_t tickSum;
    clock_t tickList[MAX];

    double updateAvg(clock_t newTick)
    {
        tickSum -= tickList[tickIndex]; // subtract value falling off
        tickSum += newTick;             // add new value
        tickList[tickIndex] = newTick;  // save new value so it can be subtracted later
        tickIndex = (tickIndex + 1) % MAX;

        return (static_cast<double>(tickSum) / MAX); // return average
    }

public:
    double fps;
    int64_t frames;

    FPSCounter() { reset(); }

    void reset()
    {
        fps = 0;
        frames = 0;
        tickIndex = 0;
        tickSum = 0;
        prevTick = 0;
        for (int i = 0; i < MAX; i++)
            tickList[i] = 0;
    }

    void tick()
    {
        frames++;
        clock_t newTick = clock();
        double avgTick = updateAvg(newTick - prevTick);
        prevTick = newTick;
        if (avgTick == 0.)
            fps = 0.0; //-1.;
        else
            fps = CLOCKS_PER_SEC / avgTick;
    }
};


namespace legacy {

struct FPSCounter
{
    clock_t start;
    clock_t end;
    int64_t frames;
    double total;
    double fps;

    FPSCounter()
    {
        reset();
    }

    void tick()
    {
        if (started())
            endFrame();
        startFrame();
    }

    void reset()
    {
        start = 0;
        end = 0;
        total = 0;
        fps = 0;
        frames = 0;
    }

    bool started()
    {
        return start != 0;
    }

    void startFrame()
    {
        start = clock();
    }

    double endFrame()
    {
        end = clock();
        total += static_cast<double>(end - start) / CLOCKS_PER_SEC;
        frames++;
        fps = (1.0 * frames) / total;
        return fps;
    }
};

} // namespace legacy


/// This class limits the throughput rate of IPackets
/// in a PacketStream. If the throughput rate exceeds the
/// max specified FPS then packets will be dropped.
///
/// Note that revious processors must not fragment packets
/// otherwise this class will not be accurate, and the packet
/// drop rate will be too high.
class AV_API FPSLimiter : public PacketProcessor
{
public:
    FPSLimiter(int max, bool videoOnly = false)
        : PacketProcessor(this->emitter)
        , _max(max)
        , _videoOnly(videoOnly)
    {
    }

    virtual void process(IPacket& packet)
    {
        // traceL("FPLLimiter", this)("Processing")

        // Proxy non video packets if videoOnly is set
        if (_videoOnly && !dynamic_cast<av::VideoPacket*>(&packet))
            emit(packet);

        if (_counter.started())
            _counter.endFrame();
        if (static_cast<int>(_counter.fps) > _max) {
            STrace << "Dropping packet: "
                   << _counter.fps << " > " << _max;
            return;
        }
        _counter.startFrame();
        emit(packet);
    }

    virtual void onStreamStateChange(const PacketStreamState&)
    {
        _counter.reset();
    }

    PacketSignal emitter;

protected:
    int _max;
    bool _videoOnly;
    legacy::FPSCounter _counter;
};


} // namespace av
} // namespace icy


/// @\}
