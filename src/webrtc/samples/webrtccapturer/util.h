///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
///


#ifndef SCY_WebRTC_Util_H
#define SCY_WebRTC_Util_H


#include "scy/webrtc/webrtc.h"
#include "scy/filesystem.h"

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif


#include "rtc_base/arraysize.h"


namespace scy {


inline std::string getEnvVarOrDefault(const char* env_var_name, const char* default_value)
{
    std::string value;
    const char* env_var = getenv(env_var_name);
    if (env_var)
        value = env_var;

    if (value.empty())
        value = default_value;

    return value;
}


inline std::string getPeerConnectionString()
{
    return getEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
}


inline std::string getDefaultServerName()
{
    return getEnvVarOrDefault("WEBRTC_SERVER", "localhost");
}


inline std::string getUsername()
{
    std::string def = "demo-" + util::randomString(6);
    return getEnvVarOrDefault("USERNAME", def.c_str());
}


inline std::string getPeerName()
{
    char computer_name[256];
    std::string ret(getUsername());
    ret += '@';
    if (gethostname(computer_name, arraysize(computer_name)) == 0) {
        ret += computer_name;
    }
    else {
        ret += "host";
    }
    return ret;
}


inline std::string getDataDirectory()
{
    std::string dir(getCwd());
    fs::addnode(dir, "data");
    return dir;
}


} // namespace scy


#endif
