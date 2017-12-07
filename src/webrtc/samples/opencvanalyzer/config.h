#ifndef SCY_WebRTC_OpenCvAnalyzer_Config_H
#define SCY_WebRTC_OpenCvAnalyzer_Config_H


#include "scy/base.h"
#include "scy/av/codec.h"
#include "scy/av/format.h"


#define SERVER_HOST "localhost"
#define USE_SSL 0 // 1
#if USE_SSL
#define SERVER_PORT 443
#else
#define SERVER_PORT 4500
#define PROCESS_EVERY_FRAME_NUMBER 10
#endif

#endif
