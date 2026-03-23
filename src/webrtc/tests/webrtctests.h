///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#pragma once


#include "icy/webrtc/codecnegotiator.h"
#include "icy/webrtc/mediabridge.h"
#include "icy/webrtc/peersession.h"
#include "icy/webrtc/track.h"
#include "icy/webrtc/trackreceiver.h"
#include "icy/webrtc/tracksender.h"

#include "icy/av/codec.h"
#include "icy/av/audiopacketencoder.h"
#include "icy/av/ffmpeg.h"
#include "icy/av/format.h"
#include "icy/av/packet.h"
#include "icy/av/videopacketencoder.h"
#include "icy/base.h"
#include "icy/logger.h"
#include "icy/test.h"

#include <rtc/rtc.hpp>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <string>
#include <vector>


using icy::test::Test;
