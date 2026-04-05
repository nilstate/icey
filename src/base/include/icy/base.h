///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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
#if defined(ICY_SHARED_LIBRARY)
#if defined(ICY_WIN)
#define ICY_EXPORT __declspec(dllexport)
#define ICY_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
#define ICY_EXPORT __attribute__((visibility("default")))
#define ICY_IMPORT __attribute__((visibility("default")))
#else
#define ICY_EXPORT
#define ICY_IMPORT
#endif
#else
#define ICY_EXPORT
#define ICY_IMPORT
#endif

#if defined(Base_EXPORTS)
#define Base_API ICY_EXPORT
#else
#define Base_API ICY_IMPORT
#endif
