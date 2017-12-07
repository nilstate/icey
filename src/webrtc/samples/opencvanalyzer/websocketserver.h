#ifndef SCY_WebSocketServer_H
#define SCY_WebSocketServer_H


#include "config.h"
#include "scy/application.h"
#include "scy/http/server.h"
#include "scy/http/websocket.h"
#include "scy/logger.h"
#include "scy/util.h"
#include "json.hpp"
#include "objectdetector.h"

namespace scy {
    namespace wrtc {
        class WebSocketServer : public http::Server {
        public:
            WebSocketServer(const net::Address &address, ThreadSafeQueue<nlohmann::json> *detectionQueue);
            virtual ~WebSocketServer();

        protected:
            void sendPayload();
            wrtc::ObjectDetector *_detector;
            Timer* _timer;
            ThreadSafeQueue<nlohmann::json> *_detectionQueue;
        };
    }
} // namespace scy


#endif
