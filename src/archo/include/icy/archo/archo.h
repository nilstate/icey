///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup archo Archive module
///
/// Archive and ZIP file handling.
/// @{


#pragma once


#include "icy/base.h"


/// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(Archo_EXPORTS)
#define Archo_API __declspec(dllexport)
#else
#define Archo_API __declspec(dllimport)
#endif
#else
#define Archo_API // nothing
#endif


/// @}
