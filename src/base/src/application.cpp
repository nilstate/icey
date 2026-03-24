///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/application.h"
#include "icy/error.h"
#include "icy/logger.h"
#include "icy/platform.h"

#include <csignal>


namespace icy {


namespace internal {

struct ShutdownCmd
{
    Application* self;
    void* opaque;
    std::function<void(void*)> callback;
};

} // namespace internal


Application& Application::getDefault()
{
    static Application instance;
    return instance;
}


Application::Application(uv::Loop* loop)
    : loop(loop)
{
    LDebug("Create");
}


Application::~Application()
{
    LDebug("Destroy");
}


void Application::run()
{
    uv_run(loop, UV_RUN_DEFAULT);
}


void Application::stop()
{
    uv_stop(loop);
}


void Application::finalize()
{
    LDebug("Finalizing");

#ifdef _DEBUG
    // Print active handles
    uv_walk(loop, Application::onPrintHandle, nullptr);
#endif

    // Run until handles are closed
    run();
    if (uv_loop_alive(loop))
        LWarn("Event loop still alive after run");
    //assert(loop->active_reqs == 0);

    LDebug("Finalization complete");
}


void Application::bindShutdownSignal(std::function<void(void*)> callback, void* opaque)
{
    auto cmd = new internal::ShutdownCmd;
    cmd->self = this;
    cmd->opaque = opaque;
    cmd->callback = callback;

    auto sig = new uv_signal_t;
    sig->data = cmd;
    uv_signal_init(loop, sig);
    uv_signal_start(sig, Application::onShutdownSignal, SIGINT);
}


void Application::waitForShutdown(std::function<void(void*)> callback, void* opaque)
{
    LDebug("Wait for shutdown");
    bindShutdownSignal(callback, opaque);
    run();
}


void Application::onShutdownSignal(uv_signal_t* req, int /* signum */)
{
    auto cmd = reinterpret_cast<internal::ShutdownCmd*>(req->data);
    LDebug("Got shutdown signal");

    uv_close(reinterpret_cast<uv_handle_t*>(req), [](uv_handle_t* handle) {
        delete handle;
    });
    if (cmd->callback)
        cmd->callback(cmd->opaque);
    delete cmd;
}


void Application::onPrintHandle(uv_handle_t* handle, void* /* arg */)
{
    LDebug("Active handle: ", handle, ": ", handle->type);
}


//
// Command-line option parser
//

OptionParser::OptionParser(int argc, char* argv[], const char* delim)
{
    char* lastkey = nullptr;
    auto dlen = strlen(delim);
    for (int i = 0; i < argc; i++) {

        // Get the application exe path (use platform API for UTF-8 support)
        if (i == 0) {
            exepath = getExePath();
            continue;
        }

        // Get option keys
        if (strncmp(argv[i], delim, dlen) == 0) {
            lastkey = (&argv[i][dlen]);
            args[lastkey] = "";
        }

        // Get value for current key
        else if (lastkey) {
            args[lastkey] = argv[i];
            lastkey = nullptr;
        }

        else {
            LDebug("Unrecognized option:", argv[i]);
        }
    }
}


} // namespace icy


/// @}
