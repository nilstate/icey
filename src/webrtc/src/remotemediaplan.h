///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#pragma once


#include "icy/webrtc/mediabridge.h"

#include <string>


namespace icy {
namespace wrtc {


[[nodiscard]] MediaBridge::Options resolveAnswerMediaOptions(
    const MediaBridge::Options& localOptions,
    const std::string& remoteOfferSdp);


} // namespace wrtc
} // namespace icy
