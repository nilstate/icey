///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/base.h"


// Shared library exports
#if defined(ICY_WIN) && defined(ICY_SHARED_LIBRARY)
#if defined(Symple_EXPORTS)
#define Symple_API __declspec(dllexport)
#else
#define Symple_API __declspec(dllimport)
#endif
#else
#define Symple_API // nothing
#endif


/// @\}
