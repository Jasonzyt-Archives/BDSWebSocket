#include "pch.h"
#include "Message.h"
#include "System.h"
#include "Mess.h"
#include "BDSWebSocket.h"
#include "ExtendedJson.h"
#include "WhiteList.h"
#include "optional.h"
#include <crow/crow_all.h>

#define OPTION(x, y) getOption<x>(msgp->data, y)
#define ARGUMENT(x, y) msgp->data.at(y).get<x>()
#define JSON_ARGUMENT(x) msgp->data.at(x)
#define EMPTY_OBJECT nlohmann::json::object()

template <typename T>
optional<T> getOption(nlohmann::json json, const std::string& key) {
	if (json.count(key)) {
		return optional(json.at(key).get<T>());
	}
	return optional<T>();
}

using namespace std;
using namespace Logger;
using WSConn = crow::websocket::connection;

nlohmann::json ConsoleLog(WSConn& conn, Message* msgp) {
	auto addr = conn.get_remote_ip() + ':' + to_string(conn.get_remote_port());
	Info() << "WS Client " << addr << ": " << ARGUMENT(string, "text") << endl;
	return EMPTY_OBJECT;
}

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
