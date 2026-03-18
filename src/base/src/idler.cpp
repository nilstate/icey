///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/idler.h"
#include "scy/logger.h"




namespace scy {


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


} // namespace scy


/// @\}
