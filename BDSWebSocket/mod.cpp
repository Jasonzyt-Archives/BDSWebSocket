#include "pch.h"
#include "Symbols.h"
#include "OfflineStorage.h"
#include "BDSWebSocket.h"
#include "WebSocketServer.h"
#include "ExtendedJson.h"
#include "Message.h"
#include "Mess.h"
#include "Crypt.h"
#include "mc/PropertiesSettings.h"
#include <mc/Player.h>
#include <loader/Loader.h>
#include <api/commands.h>
#include <api/Basic_Event.h>

#define EVENT(x) Message ev; ev.event = x
#define DATA ev.data
#define SEND bdsws->ws->sendEvent(ev)

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

// onJoin
THook(void, Symbol::ServerNetworkHandler::sendLoginMessageLocal,
	void* thiz, NetworkIdentifier* nid, void* cr, ServerPlayer* sp) {
	EVENT("onPlayerJoin");
	/*auto wt = dAccess<void*, 16>(cr);
	auto data = dAccess<JsonValue, 80>(wt);
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
	DATA["address"] = getClientAddress(nid);
	DATA["player"] = sp->getUniqueID();
	//DATA["ping"] = getPingOfPlayer(nid);
	SEND;
	bdsws->nid.emplace(sp, nid);
	original(thiz, nid, cr, sp);

	// Process offline data
	auto xuid = getXuid(sp);
	PlayerData* pld = nullptr;
	if (bdsws->ols->hasPlayer(xuid)) {
		pld = &bdsws->ols->getByXuid(xuid);
	}
	else {
		pld = &bdsws->ols->newPlayer();
	}
	pld->lastJoin = time(NULL);
	pld->name = sp->getNameTag();
	pld->realName = getRealName(sp);
	pld->xuid = xuid;
	pld->uid = sp->getUniqueID().id;
	pld->uuid = getUuid(sp).asString();
	bdsws->ols->save();
}

// onLeft
THook(void, Symbol::ServerNetworkHandler::_onPlayerLeft, void* thiz, ServerPlayer* sp) {
	EVENT("onPlayerLeft");
	DATA["player"] = sp->getUniqueID();
	SEND;
	bdsws->nid.erase(sp);
	original(thiz, sp);

	// Process offline data
	auto xuid = getXuid(sp);
	bdsws->ols->getByXuid(xuid).lastLeft = time(NULL);
	bdsws->ols->save();
}

// onChat
THook(void, Symbol::ServerNetworkHandler::handle_TextPacket, ServerNetworkHandler* thiz, 
	NetworkIdentifier* ni, void* tp) {
	auto sp = getServerPlayer(thiz, ni, dAccess<uchar, 16>(tp));
#if defined(BDS_1_16)
	auto msg = string(*(string*)((uintptr_t)tp + 80));
#elif defined(BDS_LATEST)
	auto msg = string(*(string*)((uintptr_t)tp + 88));
#endif
	EVENT("onPlayerChat");
	DATA["player"] = sp->getUniqueID();
	DATA["msg"] = msg;
	SEND;
	original(thiz, ni, tp);
}

// onDie
THook(void, Symbol::Player::die, ServerPlayer* thiz, ActorDamageSource* dsrc) {
	EVENT("onPlayerDie");
	DATA["player"] = thiz->getUniqueID();
	if (dsrc->isEntitySource()) {
		auto cause = dAccess<ActorDamageCause, 8>(dsrc);
		cout << "dmgcause: " << (int)cause << endl;
		sendText(thiz, "dmgcause: " + to_string((int)cause), TextType::RAW);
		// Source entity
		auto src = fetchActor(dsrc->getDamagingEntityUniqueID());
#if defined(BDS_1_16)
		auto hs = dAccess<HashedString, 824>(src);
#elif defined(BDS_LATEST)
		auto hs = dAccess<HashedString, 896>(src);
#endif
		auto typeName = hs.getString();
		DATA["source"] = {
			{"type", "entity"},
			{"cause", (int)cause},
			{"entity", {
				{"type", typeName},
				{"pos", src->getPos()},
				{"name", src->getNameTag()},
				{"uniqueID", src->getUniqueID().id}
			}}
		};
	}
	else {
		auto cause = dAccess<ActorDamageCause, 8>(dsrc);
		cout << "dmgcause: " << (int)cause << endl;
		sendText(thiz, "dmgcause: " + to_string((int)cause), TextType::RAW);
	}
	SEND;
	return original(thiz, dsrc);
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
