///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/loop.h"
#include "icy/runner.h"


namespace icy {


/// Run the given function at the beginning of the next event loop iteration.
template <typename Function, typename... Args>
void runOnce(uv::Loop* loop, Function&& func, Args&&... args)
{
    using Callback = internal::DeferredCallable<Function, Args...>;

    auto prepare = new uv_prepare_t;
    prepare->data = new Callback(nullptr,
                                 std::forward<Function>(func),
                                 std::forward<Args>(args)...);

    uv_prepare_init(loop, prepare);
    uv_prepare_start(prepare, [](uv_prepare_t* req) {
        auto wrap = reinterpret_cast<Callback*>(req->data);
        wrap->invoke();
        delete wrap;
        uv_prepare_stop(req);
        uv_close(reinterpret_cast<uv_handle_t*>(req), [](uv_handle_t* h) {
            delete reinterpret_cast<uv_prepare_t*>(h);
        });
    });
}


} // namespace icy


/// @\}
