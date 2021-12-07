#include "pch.h"
#include "Symbols.h"
#include "BDSWebSocket.h"
#include "WebSocketServer.h"
#include "Message.h"
#include "Mess.h"
#include "Crypt.h"
#include "mc/PropertiesSettings.h"
#include "mc/TextPacket.h"
#include <loader/Loader.h>
#include <mc/Player.h>
#include <api/commands.h>
#include <api/Basic_Event.h>

#define EVENT(x) Message ev; ev.event = x
#define DATA ev.data
#define SEND bdsws->ws->sendAll(RawMessage(ev.encryptJson()).toJson())

using namespace std;
using namespace Logger;

std::unique_ptr<BDSWebSocket> bdsws;

bool onWSDebugCmd(CommandOrigin const& ori, CommandOutput& outp, string str) {
	bdsws->ws->sendAll(str);
	return true;
}

void entry() {
	Info() << "BDSWebSocket loaded! Author: Jasonzyt" << endl;
	Info() << "Version: " << VERSIONSTR << " (Build " __DATE__ " " __TIME__ ")" << endl;
	bdsws = std::make_unique<BDSWebSocket>(Config::read());
	bdsws->ws->run(bdsws->cfg->port);
#if !defined(RELEASE)
	Event::addEventListener([&](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("wsdebug", "send a WebSocket message to ALL connected clients",
			CommandPermissionLevel::ConsoleOnly);
		CmdOverload(wsdebug, onWSDebugCmd, "message");
		}
	);
#endif
}

////////////////////////////////// 功能Hook //////////////////////////////////
struct JsonValue {
	char f[16];
};

THook(void, Symbol::ServerNetworkHandler::sendLoginMessageLocal,
	void* thiz, NetworkIdentifier* nid, void* cr, ServerPlayer* sp) {
	EVENT("onPlayerJoin");
	auto wt = dAccess<void*, 16>(cr);
	auto data = dAccess<JsonValue, 80>(wt);
	/*
	void* val = SymCall("??AValue@Json@@QEAAAEAV01@AEBV?$basic_string@DU?$"
		"char_traits@D@std@@V?$allocator@D@2@@std@@@Z", void*, void*, 
		const string&)(wt, "DeviceOS");
	cout << SymCall(Symbol::Json::Value::asString, string, 
		void*, const string&)(val, "???") << endl;
	cout << SymCall(Symbol::Json::Value::asInt, int,
		void*, int)(val, 100) << endl;*/
	//cout << SymCall("?toStyledString@Value@Json@@QEBA?AV?$basic_string"
		//"@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", string, JsonValue)
		//(data) << endl;
	//cout << SymCall("?toString@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$"
		//"char_traits@D@std@@V?$allocator@D@2@@std@@XZ", string, NetworkIdentifier*)(nid) << endl;
	DATA["realName"] = getRealName(sp);
	DATA["address"] = getClientAddress(nid);
	DATA["ping"] = getPingOfPlayer(nid);
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

THook(void, Symbol::ServerNetworkHandler::handle_TextPacket, ServerNetworkHandler* thiz, 
	NetworkIdentifier* ni, void* tp) {
	auto sp = getServerPlayer((ServerNetworkHandler*)thiz, 
		(NetworkIdentifier*)ni, dAccess<uchar, 16>(tp));
#if defined(BDS_1_16)
	auto msg = string(*(string*)((uintptr_t)tp + 80));
#elif defined(BDS_LATEST)
	auto msg = string(*(string*)((uintptr_t)tp + 88));
#endif
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

THook(void*, "??0RakPeer@RakNet@@QEAA@XZ", RakPeer_t* thiz) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		bdsws->rakpeer = thiz;
	}
	return original(thiz);
}

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
