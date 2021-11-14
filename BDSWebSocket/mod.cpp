#include "pch.h"
#include "BDSWebSocket.h"
#include "WebSocketServer.h"
#include "Message.h"
#include "Mess.h"
#include "Crypt.h"
#include <loader/Loader.h>
#include <mc/Player.h>

#define EVENT(x) Message ev; ev.event = x
#define DATA(x) ev.data[x]
#define SEND bdsws->ws->sendAll(RawMessage(ev.encryptJson()).toJson())

using namespace std;
using namespace Logger;

std::unique_ptr<BDSWebSocket> bdsws;

void entry() {
	Info() << "BDSWebSocket loaded! Author: Jasonzyt" << endl;
	Info() << "Version: " << VERSIONSTR << " (Build " __DATE__ " " __TIME__ ")" << endl;
	bdsws = std::make_unique<BDSWebSocket>(Config::read());
	bdsws->ws->run(bdsws->cfg->port);
}

////////////////////////////////// 功能Hook //////////////////////////////////

THook(void, Symbol::ServerNetworkHandler::sendLoginMessageLocal,
	void* thiz, void* ni, void* cr, ServerPlayer* sp) {
	EVENT("onPlayerJoin");
	DATA("name") = sp->getNameTag();
	DATA("xuid") = getXuid(sp);
	SEND;
	original(thiz, ni, cr, sp);
}

THook(void, Symbol::ServerNetworkHandler::_onPlayerLeft, void* thiz, ServerPlayer* sp) {
	EVENT("onPlayerLeft");
	DATA("name") = sp->getNameTag();
	DATA("xuid") = getXuid(sp);
	SEND;
	original(thiz, sp);
}

THook(void, Symbol::ServerNetworkHandler::handle_TextPacket, void* thiz, void* ni, void* tp) {
	auto sp = getServerPlayer((ServerNetworkHandler*)thiz, 
		(NetworkIdentifier*)ni, dAccess<uchar, 16>(tp));
	auto msg = std::string(*dAccess<std::string*, 88>(tp));
	EVENT("onPlayerChat");
	DATA("name") = sp->getNameTag();
	DATA("xuid") = getXuid(sp);
	DATA("msg") = msg;
	SEND;
	original(thiz, ni, tp);
}

THook(void, Symbol::ServerInstance::startServerThread, void* thiz) {
	original(thiz);
	bdsws->level = bdsws->mc->getLevel();
	EVENT("onServerStarted");
	DATA("IPv4Port") = bdsws->ipv4Port;
	DATA("IPv6Port") = bdsws->ipv6Port;
	SEND;
}

////////////////////////////////// 工具Hook //////////////////////////////////

THook(uint16_t, Symbol::RakNetInstance::getIPv4Port, void* thiz) {
	return bdsws->ipv4Port = original(thiz);
}

THook(uint16_t, Symbol::RakNetInstance::getIPv6Port, void* thiz) {
	return bdsws->ipv6Port = original(thiz);
}

THook(void, Symbol::Minecraft::initAsDedicatedServer, Minecraft* thiz) {
	bdsws->mc = thiz;
	original(thiz);
}

THook(int, Symbol::WhitelistFile::reload, void* thiz) {
	bdsws->wlfile = thiz;
	return original(thiz);
}

THook(void, Symbol::ServerLevel::tick, void* thiz) {
	bdsws->ws->processTask(); // 处理任务队列
	original(thiz);
}
