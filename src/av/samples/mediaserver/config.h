#ifndef SCY_MediaServer_Config_H
#define SCY_MediaServer_Config_H

#include "scy/application.h"
#include "scy/av/multiplexencoder.h"
#include "scy/http/server.h"
#include "scy/logger.h"
#include "scy/packetstream.h"
#include "scy/util.h"


namespace scy {


const std::string kRelayServerIP = "127.0.0.1"; // TURN server IP

#define SERVER_PORT 1328


} // namespace scy


#endif // SCY_MediaServer_Config_H