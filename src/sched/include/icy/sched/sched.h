///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
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
#if defined(Sched_EXPORTS)
#define Sched_API ICY_EXPORT
#else
#define Sched_API ICY_IMPORT
#endif


/// @}
