///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup webrtc WebRTC module
///
/// WebRTC media transport via libdatachannel; peer sessions, media bridge, codec negotiation.
/// @{


#pragma once


#include "icy/base.h"


// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(WebRTC_EXPORTS)
#define WEBRTC_API __declspec(dllexport)
#else
#define WEBRTC_API __declspec(dllimport)
#endif
#else
#define WEBRTC_API // nothing
#endif


/// @}
