#ifndef BDSWEBSOCKET_H
#define BDSWEBSOCKET_H
#include "pch.h"
#include "Config.h"
#include "LangPack.h"
#include "WebSocketServer.h"

class BDSWebSocket {

public:

	std::unique_ptr<Config> cfg;
	std::unique_ptr<LangPack> lpk;
	std::unique_ptr<WebSocketServer> ws;

	BDSWebSocket(Config* cfg) {
		this->cfg.reset(cfg);
		this->lpk.reset(new LangPack(PLUGIN_LANGPK, cfg->language));
		this->ws.reset(new WebSocketServer);
	}

};

#endif // !BDSWEBSOCKET_H
