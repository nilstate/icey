///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/format.h"
#include <algorithm>
#include <sstream>




namespace icy {
namespace av {


Format::Format()
    : priority(0)
{
}


Format::Format(const std::string& name, const std::string& id,
               const VideoCodec& video, const AudioCodec& audio, int priority)
    : name(name)
    , id(id)
    , video(video)
    , audio(audio)
    , priority(priority)
{
}


Format::Format(const std::string& name, const std::string& id,
               const VideoCodec& video, int priority)
    : name(name)
    , id(id)
    , video(video)
    , priority(priority)
{
}


Format::Format(const std::string& name, const std::string& id,
               const AudioCodec& audio, int priority)
    : name(name)
    , id(id)
    , audio(audio)
    , priority(priority)
{
}


Format::Format(const Format& r)
    : name(r.name)
    , id(r.id)
    , video(r.video)
    , audio(r.audio)
    , priority(r.priority)
{
}


Format::Type Format::type() const
{
    if (video.enabled && audio.enabled)
        return Format::Multiplex;
    else if (video.enabled)
        return Format::Video;
    else if (audio.enabled)
        return Format::Audio;
    else
        return Format::None;
}


std::string Format::toString() const
{
    std::ostringstream os;
    os << "Format[" << name << ":" << id;
    if (video.enabled)
        os << "\n\t" << video.toString();
    if (audio.enabled)
        os << "\n\t" << audio.toString();
    os << "]";
    return os.str();
}


void Format::print(std::ostream& ost)
{
    ost << "Format[" << name << ":" << id;
    if (video.enabled) {
        ost << "\n";
        video.print(ost);
        // ost << "\n";
    }
    if (audio.enabled) {
        ost << "\n";
        audio.print(ost);
        // ost << "\n";
    }
    ost << "\n]";
}


} // namespace av
} // namespace icy


/// @}
