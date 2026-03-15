///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "scy/base.h"


// Shared library exports
#if defined(SCY_WIN) && defined(SCY_SHARED_LIBRARY)
#if defined(Symple_EXPORTS)
#define Symple_API __declspec(dllexport)
#else
#define Symple_API __declspec(dllimport)
#endif
#else
#define Symple_API // nothing
#endif


/// @\}
