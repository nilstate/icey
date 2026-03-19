///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup base Base module
///
/// The `base` module contains reusable cross platform tools and utilities.


#pragma once


// Include build config
#include "icey.h"

// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(Base_EXPORTS)
#define Base_API __declspec(dllexport)
#else
#define Base_API __declspec(dllimport)
#endif
#else
#define Base_API // nothing
#endif
