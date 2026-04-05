///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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
#if defined(HTTP_EXPORTS)
#define HTTP_API ICY_EXPORT
#else
#define HTTP_API ICY_IMPORT
#endif

/// @}
