#include "pch.h"
#include "BDSWebSocket.h"
#include "WebSocketServer.h"
#include "Message.h"
#include "Mess.h"
#include "Crypt.h"
#include "mc/PropertiesSettings.h"
#include <loader/Loader.h>
#include <mc/Player.h>

#define EVENT(x) Message ev; ev.event = x
#define DATA ev.data
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
	void* thiz, NetworkIdentifier* nid, void* cr, ServerPlayer* sp) {
	EVENT("onPlayerJoin");
	DATA["realName"] = getRealName(sp);
	DATA["address"] = getClientAddress(nid);
	DATA["name"] = sp->getNameTag();
	DATA["xuid"] = getXuid(sp);
	SEND;
	bdsws->nid.emplace(sp, nid);
	original(thiz, nid, cr, sp);
}

THook(void, Symbol::ServerNetworkHandler::_onPlayerLeft, void* thiz, ServerPlayer* sp) {
	EVENT("onPlayerLeft");
	DATA["realName"] = getRealName(sp);
	DATA["name"] = sp->getNameTag();
	DATA["xuid"] = getXuid(sp);
	SEND;
	bdsws->nid.erase(sp);
	original(thiz, sp);
}

THook(void, Symbol::ServerNetworkHandler::handle_TextPacket, void* thiz, void* ni, void* tp) {
	auto sp = getServerPlayer((ServerNetworkHandler*)thiz, 
		(NetworkIdentifier*)ni, dAccess<uchar, 16>(tp));
	auto msg = std::string(*dAccess<std::string*, 88>(tp));
	EVENT("onPlayerChat");
	DATA["name"] = sp->getNameTag();
	DATA["xuid"] = getXuid(sp);
	DATA["msg"] = msg;
	SEND;
	original(thiz, ni, tp);
}

THook(void, Symbol::ServerInstance::startServerThread, void* thiz) {
	original(thiz);
	nlohmann::json spts; // Server properties
	bdsws->level = bdsws->mc->getLevel();
	EVENT("onServerStarted");
	DATA["IPv4Port"] = bdsws->ipv4Port;
	DATA["IPv6Port"] = bdsws->ipv6Port;
	// 服务器配置
	spts["serverName"]   = bdsws->properties->server_name;
	spts["levelName"]    = bdsws->properties->level_name;
	spts["levelSeed"]    = bdsws->properties->level_seed; // 若未填则为空
	spts["onlineMode"]   = bdsws->properties->online_mode;
	spts["maxPlayers"]   = bdsws->properties->max_players;
	spts["tickDistance"] = bdsws->properties->tick_distance;
	spts["viewDistance"] = bdsws->properties->view_distance;
	spts["allowCheats"]  = bdsws->properties->allow_cheats;
	spts["difficulty"]   = bdsws->properties->difficulty;
	spts["gamemode"]     = bdsws->properties->gamemode;
	spts["language"]     = bdsws->properties->language;
	DATA["serverProperties"] = spts;
	SEND;
}

////////////////////////////////// 工具Hook //////////////////////////////////

THook(void, Symbol::PropertiesSettings::PropertiesSettings, 
	PropertiesSettings* thiz, const std::string& file) {
	original(thiz, file); // 先执行构造函数
	bdsws->properties = thiz;
}

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
