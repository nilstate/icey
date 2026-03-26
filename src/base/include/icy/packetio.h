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


#include "icy/base.h"
#include "icy/packetstream.h"
#include <fstream>
#include <iostream>


namespace icy {


///
/// Threaded stream reader class.
///
/// This class can be connected to a `PacketStream` to read input from any
/// class that derives from `std::istream`. It's most regularly used for reading
/// input files.
///
class ThreadedStreamReader : public PacketSource
    , public basic::Startable
{
public:
    /// @param is Input stream to read from; takes ownership.
    ThreadedStreamReader(std::istream* is)
        : PacketSource(this->emitter)
        , _istream(is)
    {
        _runner.setRepeating(true);
    }

    /// Stops the reader thread and deletes the owned stream.
    ~ThreadedStreamReader()
    {
        stop();

        if (_istream) {
            delete _istream;
        }
    }

    /// Starts the reader thread; emits one line per iteration as a RawPacket.
    /// Emits a FlagPacket with `PacketFlags::Final` on EOF.
    void start() override
    {
        _runner.start([](void* arg) {
            auto self = reinterpret_cast<ThreadedStreamReader*>(arg);
            std::string line;
            if (getline(self->stream(), line)) {
                self->emit(line);
            }
            if (self->stream().eof()) {
                self->emit(static_cast<unsigned>(PacketFlags::Final));
            }
        },
                      this);
    }

    /// Cancels the reader thread.
    void stop() override
    {
        _runner.cancel();
    }

    /// Returns the internal stream cast to `StreamT`.
    /// @tparam StreamT Target stream type derived from `std::istream`.
    /// @return Reference to the cast stream.
    /// @throws std::runtime_error if the cast fails.
    template <class StreamT>
    StreamT& stream()
    {
        auto stream = dynamic_cast<StreamT*>(_istream);
        if (!stream)
            throw std::runtime_error("Cannot cast internal stream type.");

        return *stream;
    }

    /// @return Reference to the underlying input stream.
    std::istream& stream() { return *_istream; }

    PacketSignal emitter;

protected:
    Thread _runner;
    std::istream* _istream;
};


///
/// Packet stream writer class.
///
/// This class can be connected to a `PacketStream` to write output to any
/// class that derives from `std::ostream`. It's most regularly used for writing
/// output files.
///
class StreamWriter : public PacketProcessor
{
public:
    /// @param stream Output stream to write to; takes ownership.
    StreamWriter(std::ostream* stream)
        : PacketProcessor(this->emitter)
        , _ostream(stream)
    {
    }

    /// Closes any open `std::ofstream` and deletes the owned stream.
    virtual ~StreamWriter()
    {
        if (_ostream) {
            auto fstream = dynamic_cast<std::ofstream*>(_ostream);
            if (fstream)
                fstream->close();
            delete _ostream;
        }
    }

    /// Serializes the packet via `write()`, flushes it to the output stream,
    /// then forwards the packet to the next processor.
    /// @param packet Incoming packet to process.
    virtual void process(IPacket& packet) override
    {
        Buffer buffer;
        packet.write(buffer);

        _ostream->write(buffer.data(), buffer.size());
        _ostream->flush();

        emit(packet);
    }

    /// Returns the internal output stream cast to `StreamT`.
    /// @tparam StreamT Target stream type derived from `std::ostream`.
    /// @return Reference to the cast stream.
    /// @throws std::runtime_error if the cast fails.
    template <class StreamT>
    StreamT& stream()
    {
        auto stream = dynamic_cast<StreamT*>(_ostream);
        if (!stream)
            throw std::runtime_error("Cannot cast internal stream type.");

        return *stream;
    }

    /// Closes the output file on `Closed` or `Error` stream state transitions.
    /// @param state New stream state.
    void onStreamStateChange(const PacketStreamState& state) override
    {
        //LTrace("Stream state: ", state);

        switch (state.id()) {
            // case PacketStreamState::None:
            // case PacketStreamState::Active:
            // case PacketStreamState::Resetting:
            // case PacketStreamState::Stopping:
            // case PacketStreamState::Stopped:
            case PacketStreamState::Closed:
            case PacketStreamState::Error:
                // Close file handles
                auto fstream = dynamic_cast<std::ofstream*>(_ostream);
                if (fstream)
                    fstream->close();
                break;
        }
    }

    /// @return Reference to the underlying output stream.
    /// @throws std::runtime_error if the stream pointer is null.
    std::ostream& stream()
    {
        if (!_ostream)
            throw std::runtime_error("Cannot cast internal stream type.");
        return *_ostream;
    }

    PacketSignal emitter;

protected:
    std::ostream* _ostream;
};


} // namespace icy


/// @}
