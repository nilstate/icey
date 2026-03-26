///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/synchronizer.h"


namespace icy {


Synchronizer::Synchronizer(uv::Loop* loop)
    : _handle(loop)
{
}


Synchronizer::Synchronizer(std::function<void()> target, uv::Loop* loop)
    : _handle(loop)
{
    start<std::function<void()>>(std::forward<std::function<void()>>(target));
}


Synchronizer::~Synchronizer()
{
    close();
}


void Synchronizer::start(std::function<void()> target)
{
    start<std::function<void()>>(std::forward<std::function<void()>>(target));
}


void Synchronizer::post()
{
    if (!_handle.initialized()) return;
    // NOTE: Cannot call `_handle.get()` as we're on different thread
    uv_async_send(_handle.context()->ptr);
}


void Synchronizer::cancel()
{
    Runner::cancel();
}


void Synchronizer::close()
{
    if (!_handle.initialized())
        return;
    cancel();
    post(); // post to wake up event loop
    _handle.close();
}


// bool Synchronizer::closed()
// {
//     return !_handle.initialized();
//     // return _handle.closed();
// }


bool Synchronizer::async() const
{
    return false;
}


uv::Handle<uv_async_t>& Synchronizer::handle()
{
    return _handle;
}


} // namespace icy


/// @}
