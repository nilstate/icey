///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup turn TURN module (rfc5766)
///
/// The `turn` module contains TURN (rfc5766) client and server implementations.
/// @{

#pragma once

#include "icy/base.h"


// Shared library exports
#if defined(TURN_EXPORTS)
#define TURN_API ICY_EXPORT
#else
#define TURN_API ICY_IMPORT
#endif


/// @}
