///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/pipe.h"




namespace icy {


Pipe::Pipe(uv::Loop* loop)
    : Stream<uv_pipe_t>(loop) //, new uv_pipe_t
{
}


Pipe::~Pipe()
{
}


void Pipe::init(bool ipc)
{
    // uv_pipe_init(loop(), ptr<uv_pipe_t>(), ipc ? 1 : 0);
    // Stream::init();
    // <uv_pipe_t>
    Stream<uv_pipe_t>::init(&uv_pipe_init, ipc ? 1 : 0);
}


bool Pipe::readStart()
{
    return Stream<uv_pipe_t>::readStart();
}


// bool Pipe::readStop()
// {
//     return Stream::readStop();
// }


} // namespace icy


/// @}
