///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup turn TURN module (rfc5766)
///
/// The `turn` module contains TURN (rfc5766) client and server implementations.
/// @{

#pragma once


// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(TURN_EXPORTS)
#define TURN_API __declspec(dllexport)
#else
#define TURN_API __declspec(dllimport)
#endif
#else
#define TURN_API // nothing
#endif


/// @}
