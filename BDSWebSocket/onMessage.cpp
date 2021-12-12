#include "pch.h"
#include "option.h"
#include "Message.h"
#include "System.h"
#include "Mess.h"
#include "BDSWebSocket.h"
#include "ExtendedJson.h"
#include "OfflineStorage.h"
#include "WhiteList.h"
#include <crow/crow_all.h>

#define OPTION(x, y) getOption<x>(msgp->data, y)
#define ARGUMENT(x, y) msgp->data.at(y).get<x>()
#define JSON_ARGUMENT(x) msgp->data.at(x)
#define EMPTY_OBJECT nlohmann::json::object()

template <typename T>
option<T> getOption(nlohmann::json json, const std::string& key) {
	if (json.count(key)) {
		return option(json.at(key).get<T>());
	}
	return option<T>();
}

using namespace std;
using namespace Logger;
using WSConn = crow::websocket::connection;

nlohmann::json hello(WSConn& conn, Message* msgp) {
	auto addr = conn.get_remote_ip() + ':' + to_string(conn.get_remote_port());
	auto& settings = bdsws->ws->getClientSettings(addr);
	settings.name = ARGUMENT(string, "name");
	settings.intro = ARGUMENT(string, "introduction");
	settings.others = ARGUMENT(nlohmann::json, "others");
	settings.xuidString = ARGUMENT(bool, "xuidString");
	return EMPTY_OBJECT;
}

// {"id":"123", "type":"consoleLog", "data":{"text":"awa"}}
nlohmann::json ConsoleLog(WSConn& conn, Message* msgp) {
	auto addr = conn.get_remote_ip() + ':' + to_string(conn.get_remote_port());
	Info() << "WS Client " << addr << '(' << bdsws->ws->getClientSettings(addr).name 
		<< "): " << ARGUMENT(string, "text") << endl;
	return EMPTY_OBJECT;
}

// {"id":"123", "type":"getPerformUsages"}
nlohmann::json getPerformanceUsages(WSConn& conn, Message* msgp) {
	nlohmann::json data;
	// CPU
	data["cpu"]["rate"] = bdsws->gcu.getCurrentValue();
	data["cpu"]["totalRate"] = bdsws->gcu.getCurrentValueTotal();
	data["currentProcessCpu"]["totalRate"] = bdsws->cpcu.getCurrentValue();
	// Memory
	data["memory"] = getMemoryUsage();
	data["currentProcessMemory"] = getCurrentProcessMemoryUsage();
	// Disk
	auto bufsize = GetLogicalDriveStringsA(NULL, NULL);
	char* buf = new char[bufsize];
	// C:\+\0+D:\+\0\0
	GetLogicalDriveStringsA(bufsize, buf);
	string str(buf);
	str = str.substr(0, str.size() - 2);
	auto dNames = split(str, '\0');
	for (auto& dName : dNames) {
		data["disks"][dName.substr(0, 1)] = getDiskUsage(dName[0]);
	}
	return data;
}

nlohmann::json getWebSocketClients(WSConn& conn, Message* msgp) {
	nlohmann::json data;
	
	return data;
}

nlohmann::json addWhiteList(WSConn& conn, Message* msgp) {
	auto name = ARGUMENT(string, "name");
	auto xuid = OPTION(xuid_t, "xuid");
	auto ignores = OPTION(bool, "ignoresPlayerLimit");
	WhiteList().add(name, (xuid.set() ? xuid.val() : 0), 
			(ignores.set() ? ignores.val() : false)).reload();
	return EMPTY_OBJECT;
}

nlohmann::json removeWhiteList(WSConn& conn, Message* msgp) {
	auto name = OPTION(string, "name");
	auto xuid = OPTION(xuid_t, "xuid");
	if (xuid.set()) {
		WhiteList().remove(xuid.val()).reload();
	}
	else if (name.set()) {
		WhiteList().remove(name.val()).reload();
	}
	return EMPTY_OBJECT;
}

nlohmann::json getWhiteList(WSConn& conn, Message* msgp) {
	nlohmann::json data;
	auto ls = WhiteList().list();
	for (auto& [name, xuid] : ls) {
		data["list"].push_back({ {"name", name}, {"xuid", xuid} });
	}
	return data;
}

// {"id":"123", "type":"getOnlinePlayers"}
nlohmann::json getOnlinePlayers(WSConn& conn, Message* msgp) {
	nlohmann::json data;
	auto ls = getAllPlayers();
	for (auto& pl : ls) {
		data["list"].push_back(pl->getUniqueID());
	}
	return data;
}

// {"id":"123", "type":"getOnlinePlayerInfo", "data":{"player":{...}}}
nlohmann::json getOnlinePlayerInfo(WSConn& conn, Message* msgp) {
	nlohmann::json data;
	auto pl = ARGUMENT(Player*, "player");
	if (pl) {
		data["player"] = pl->getUniqueID(); // See ExtendedJson.cpp
	}
	else throw buildException("Player not found!");
	return data;
}

nlohmann::json getOfflinePlayerInfo(WSConn& conn, Message* msgp) {
	nlohmann::json data;
	auto xuid = OPTION(xuid_t, "xuid");
	auto uuid = OPTION(string, "uuid");
	auto realName = OPTION(string, "realName");
	PlayerData* pdata = nullptr;
	if (xuid.set()) {
		pdata = &bdsws->ols->getByXuid(xuid.val());
	}
	else if (uuid.set()) {
		pdata = &bdsws->ols->getByUuid(uuid.val());
	}
	else if (realName.set()) {
		pdata = &bdsws->ols->getByRealName(realName.val());
	}
	if (pdata) {
		data = *pdata;
		data["online"] = (pdata->lastJoin > pdata->lastLeft);
	}
	else {
		throw buildException("Offline player not found!");
	}
	return data;
}

nlohmann::json sendTextToPlayer(WSConn& conn, Message* msgp) {
	auto pl = ARGUMENT(Player*, "player");
	auto text = ARGUMENT(string, "text");
	auto type = ARGUMENT(int, "textType");
	if (pl) {
		sendText(pl, text, (TextType)type);
	}
	else throw buildException("Player not found!");
	return EMPTY_OBJECT;
}

nlohmann::json executeCmd(WSConn& conn, Message* msg) {
	return EMPTY_OBJECT;
}
