///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup http HTTP module
///
/// HTTP client/server, WebSocket support, form handling, cookies, URL parsing.
/// @{

#pragma once

#include "icy/base.h"


// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(HTTP_EXPORTS)
#define HTTP_API __declspec(dllexport)
#else
#define HTTP_API __declspec(dllimport)
#endif
#else
#define HTTP_API // nothing
#endif

/// @}
