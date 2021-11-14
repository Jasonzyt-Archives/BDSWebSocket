#include "pch.h"
#include "BDSWebSocket.h"
#include "WebSocketServer.h"
#include "Message.h"
#include "Mess.h"
#include <loader/Loader.h>
#include <mc/Player.h>

using namespace std;
using namespace Logger;

std::unique_ptr<BDSWebSocket> bdsws;

void entry() {
	Info() << "BDSWebSocket loaded! Author: Jasonzyt" << endl;
	Info() << "Version: " << VERSIONSTR << " (Build " __DATE__ " " __TIME__ ")" << endl;
	bdsws = std::make_unique<BDSWebSocket>(Config::read());
	bdsws->ws->run(bdsws->cfg->port);
}

THook(void, Symbol::ServerNetworkHandler::sendLoginMessageLocal, 
	void* thiz, void* cr, ServerPlayer* sp) {
	Message msg;
	msg.event = "chat";
	msg.data["name"] = sp->getNameTag();
	msg.data["xuid"] = getXuid(sp);
	bdsws->ws->sendAll(RawMessage(msg.encryptJson()).toJson());
	original(thiz, cr, sp);
}

THook(void, Symbol::ServerLevel::tick, void* thiz) {
	bdsws->ws->processQueue();
	original(thiz);
}

THook(void, Symbol::Minecraft::initAsDedicatedServer, Minecraft* thiz) {
	bdsws->mc = thiz;
	original(thiz);
}

THook(void, Symbol::ServerInstance::startServerThread, void* thiz) {
	original(thiz);
	bdsws->level = bdsws->mc->getLevel();
}

THook(int, Symbol::WhitelistFile::reload, void* thiz) {
	bdsws->wlfile = thiz;
	return original(thiz);
}
