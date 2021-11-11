#include "pch.h"
#include "BDSWebSocket.h"
#include "WebSocketServer.h"
#include <loader/Loader.h>
#include <mc/Player.h>

using namespace std;
using namespace Logger;

std::unique_ptr<BDSWebSocket> bdsws;

void entry() {
	cout << "BDS WebSocket loaded! Author: Jasonzyt" << endl;
	bdsws = std::make_unique<BDSWebSocket>(Config::read());
	runWebSocketServer(bdsws->cfg->port);
}

THook(void, Symbol::ServerNetworkHandler_sendLoginMessageLocal, 
	void* thiz, void* cr, ServerPlayer* sp) {
	
	original(thiz, cr, sp);
}
