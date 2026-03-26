///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/idler.h"
#include "icy/logger.h"




namespace icy {


Idler::Idler(uv::Loop* loop)
    : _handle(loop)
{
    init();
}


void Idler::init()
{
    _handle.init(&uv_idle_init);
    _handle.unref(); // unref by default
}


void Idler::start(std::function<void()> target)
{
    start<std::function<void()>>(std::forward<std::function<void()>>(target));
}


uv::Handle<uv_idle_t>& Idler::handle()
{
    return _handle;
}


bool Idler::async() const
{
    return false;
}


} // namespace icy


/// @}
