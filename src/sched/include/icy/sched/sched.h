///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @defgroup sched Sched module
///
/// Task scheduler for deferred and periodic jobs.
/// @{


#pragma once


#include "icy/base.h"


// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(Sched_EXPORTS)
#define Sched_API __declspec(dllexport)
#else
#define Sched_API __declspec(dllimport)
#endif
#else
#define Sched_API // nothing
#endif


/// @\}
