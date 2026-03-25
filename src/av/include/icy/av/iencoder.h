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


#include "icy/av/format.h"
#include "icy/packetstream.h"
#include "icy/signal.h"
#include "icy/stateful.h"


namespace icy {
namespace av {


/// State machine states for the encoder pipeline
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


/// Configuration options for audio and video encoders
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

    /// Initialise the encoder, open codec contexts, and transition to Ready or Encoding state.
    virtual void init() = 0;

    /// Flush remaining packets, close codec contexts, and release all encoder resources.
    virtual void uninit() = 0;

    /// Perform any additional cleanup after uninit(). Default is a no-op.
    virtual void cleanup() {}

    /// @return A reference to the encoder's configuration options.
    virtual EncoderOptions& options() = 0;

    /// Initialise the video codec context and stream.
    virtual void createVideo() {}

    /// Free the video codec context and stream.
    virtual void freeVideo() {}

    /// Encode a single AVFrame of video.
    /// @param frame  The source video frame; all fields (format, width, height, pts) must be set.
    /// @return True if a packet was successfully encoded and written.
    [[nodiscard]] virtual bool encodeVideo(AVFrame* frame) { (void)frame; return false; }

    /// Initialise the audio codec context and stream.
    virtual void createAudio() {}

    /// Free the audio codec context and stream.
    virtual void freeAudio() {}

    /// Flush any internally buffered packets to the output.
    virtual void flush() {}

    /// @return True if the encoder is in the None state.
    bool isNone() const { return stateEquals(EncoderState::None); }

    /// @return True if the encoder is ready but not yet encoding.
    bool isReady() const { return stateEquals(EncoderState::Ready); }

    /// @return True if the encoder is actively encoding.
    virtual bool isEncoding() const { return stateEquals(EncoderState::Encoding); }

    /// @return True if the encoder is in the Ready or Encoding state.
    virtual bool isActive() const { return stateBetween(EncoderState::Ready, EncoderState::Encoding); }

    /// @return True if the encoder has stopped.
    virtual bool isStopped() const { return stateEquals(EncoderState::Stopped); }

    /// @return True if the encoder is in an error state.
    virtual bool isError() const { return stateEquals(EncoderState::Error); }
};


using IPacketEncoder = IEncoder; ///< Legacy alias for IEncoder kept for 0.8.x compatibility.


} // namespace av
} // namespace icy


/// @}
