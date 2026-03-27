///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup speech Speech module
///
/// Audio intelligence primitives for decoded media streams.
/// @{


#pragma once


#include "icy/base.h"


#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(Speech_EXPORTS)
#define Speech_API __declspec(dllexport)
#else
#define Speech_API __declspec(dllimport)
#endif
#else
#define Speech_API
#endif


/// @}
