///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup webrtc
/// @{


#pragma once


#include "scy/base.h"


// Shared library exports
#if defined(SCY_WIN) && defined(SCY_SHARED_LIBRARY)
#if defined(WebRTC_EXPORTS)
#define WEBRTC_API __declspec(dllexport)
#else
#define WEBRTC_API __declspec(dllimport)
#endif
#else
#define WEBRTC_API // nothing
#endif


/// @\}
