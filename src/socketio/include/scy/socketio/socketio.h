///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup socketio
/// @{

#pragma once

#include "scy/base.h"


// Shared library exports
#if defined(SCY_WIN) && defined(SCY_SHARED_LIBRARY)
#if defined(SocketIO_EXPORTS)
#define SocketIO_API __declspec(dllexport)
#else
#define SocketIO_API __declspec(dllimport)
#endif
#else
#define SocketIO_API // nothing
#endif


/// @}
