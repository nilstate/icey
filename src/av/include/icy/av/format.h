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


#include "icy/av/codec.h"

#include <iostream>
#include <string>
#include <vector>


namespace icy {
namespace av {

/// Defines a media container format which is available through the
/// Format Registry for encoding/decoding. A format defined preferred
/// default values for each codec.
struct AV_API Format
{
    enum Type
    {
        None = 0,
        Video = 1,    ///< video only
        Audio = 2,    ///< audio only
        Multiplex = 3 ///< both video & audio
    };

    ///
    /// Base Format Variables
    //
    std::string name; ///< The display name of this format.
    std::string id;   ///< The short name of this format.

    VideoCodec video; ///< The video codec.
    AudioCodec audio; ///< The audio codec.

    int priority; ///< The priority this format will be displayed on the list.

    ///
    /// Ctors/Dtors
    //

    /// Construct an empty format with no codecs enabled.
    Format();

    /// Construct a multiplex (audio + video) format.
    /// @param name      The display name of the format.
    /// @param id        The short FFmpeg muxer ID (e.g. "mp4").
    /// @param video     The video codec parameters.
    /// @param audio     The audio codec parameters.
    /// @param priority  Sort priority; higher values appear first.
    Format(const std::string& name,
           const std::string& id, // Multiplex format constructor
           const VideoCodec& video, const AudioCodec& audio, int priority = 0);

    /// Construct a video-only format.
    /// @param name      The display name of the format.
    /// @param id        The short FFmpeg muxer ID.
    /// @param video     The video codec parameters.
    /// @param priority  Sort priority; higher values appear first.
    Format(const std::string& name,
           const std::string& id, // Video only format constructor
           const VideoCodec& video, int priority = 0);

    /// Construct an audio-only format.
    /// @param name      The display name of the format.
    /// @param id        The short FFmpeg muxer ID.
    /// @param audio     The audio codec parameters.
    /// @param priority  Sort priority; higher values appear first.
    Format(const std::string& name,
           const std::string& id, // Audio only format constructor
           const AudioCodec& audio, int priority = 0);


    Format(const Format& r);
    Format& operator=(const Format&) = default;

    virtual ~Format() = default;

    /// Return the media type (None, Video, Audio, or Multiplex) derived from which codecs are enabled.
    Type type() const;

    /// @return A compact string representation of the format and its enabled codecs.
    virtual std::string toString() const;

    /// Print a multi-line human-readable description to the given stream.
    /// @param ost  The output stream to write to.
    virtual void print(std::ostream& ost);

    /// Comparator returning true if @p first has higher priority than @p second.
    /// @param first   The format to compare.
    /// @param second  The format to compare against.
    /// @return True if first.priority > second.priority.
    static bool preferable(const Format& first, const Format& second)
    {
        return first.priority > second.priority;
    }
};


using FormatList = std::vector<Format>;
using FormatPList = std::vector<Format*>;


} // namespace av
} // namespace icy
