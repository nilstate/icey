///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include "scy/stream.h"


namespace scy {


/// Pipe implementation for process stdio
class Base_API Pipe : public Stream<uv_pipe_t>
{
public:
    Pipe(uv::Loop* loop = uv::defaultLoop());
    virtual ~Pipe();

    virtual void init(bool ipc = false);

    bool readStart() override;
    //virtual bool readStop() override;
};


} // namespace scy


/// @\}
