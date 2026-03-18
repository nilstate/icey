///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#pragma once


#include "scy/av/format.h"
#include "scy/packetstream.h"
#include "scy/signal.h"
#include "scy/stateful.h"


namespace scy {
namespace av {


struct EncoderState : public State
{
    enum Type
    {
        None = 0,
        Ready,
        Encoding,
        Stopped,
        Error
    };

    std::string str(unsigned int id) const
    {
        switch (id) {
            case None:
                return "None";
            case Ready:
                return "Ready";
            case Encoding:
                return "Encoding";
            case Stopped:
                return "Stopped";
            case Error:
                return "Error";
        }
        return "undefined";
    };
};


struct EncoderOptions
{
    Format iformat;    ///< input media format.
    Format oformat;    ///< output media format.
    std::string ifile; ///< input file path.
    std::string ofile; ///< output file path.
    long duration;     ///< duration of time to record in nanoseconds.
    EncoderOptions(const Format& iformat = Format(),
                   const Format& oformat = Format(),
                   const std::string& ifile = "",
                   const std::string& ofile = "",
                   long duration = 0)
        : iformat(iformat)
        , oformat(oformat)
        , ifile(ifile)
        , ofile(ofile)
        , duration(duration)
    {
    }

    virtual ~EncoderOptions() = default;
};

/// This is the abstract class for all encoders.
class AV_API IEncoder : public Stateful<EncoderState>
{
public:
    enum Type
    {
        None = 0,     ///< huh?
        Video = 1,    ///< video only
        Audio = 2,    ///< audio only
        Multiplex = 3 ///< both video & audio
    };

    virtual void init() = 0;
    virtual void uninit() = 0;

    virtual EncoderOptions& options() = 0;

    bool isNone() const { return stateEquals(EncoderState::None); } override;
    bool isReady() const { return stateEquals(EncoderState::Ready); } override;
    virtual bool isEncoding() const { return stateEquals(EncoderState::Encoding); }
    virtual bool isActive() const { return stateBetween(EncoderState::Ready, EncoderState::Encoding); }
    virtual bool isStopped() const { return stateEquals(EncoderState::Stopped); }
    virtual bool isError() const { return stateEquals(EncoderState::Error); }
};


using IPacketEncoder = IEncoder; ///< 0.8.x compatibility


} // namespace av
} // namespace scy


/// @\}
