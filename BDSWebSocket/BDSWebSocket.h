#ifndef BDSWEBSOCKET_H
#define BDSWEBSOCKET_H
#include "pch.h"
#include "Config.h"
#include "LangPack.h"

namespace crow {
	namespace websocket {
		class connection;
	}
}
class Message;

class BDSWebSocket {

public:

	std::unique_ptr<Config> cfg;
	std::unique_ptr<LangPack> lpk;
	// WebSocketServer
	std::queue<std::function<void(const Message&)>> exec_queue;
	std::unordered_map<std::string, crow::websocket::connection&> clients;
	std::mutex ws_mtx;

	BDSWebSocket(Config* cfg) {
		this->cfg.reset(cfg);
		this->lpk.reset(new LangPack(PLUGIN_LANGPK, cfg->language));
	}

};

#endif // !BDSWEBSOCKET_H
