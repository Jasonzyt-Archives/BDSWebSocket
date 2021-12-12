#ifndef ONMESSAGE_H
#define ONMESSAGE_H
#include "pch.h"
#include <crow/crow_all.h>
#include <nlohmann/json.hpp>

using WSConn = crow::websocket::connection;

class Message;

struct onMessage {
	std::function<nlohmann::json(WSConn&, Message*)> cb;
	std::vector<std::string> args;
	std::vector<std::string> option; // 多选一,一个也没有会报错
};

#define CALLBACK(x) nlohmann::json x(WSConn& conn, Message* msgp)

CALLBACK(hello);
CALLBACK(ConsoleLog);
CALLBACK(getPerformanceUsages);
CALLBACK(getWebSocketClients);

CALLBACK(addWhiteList);
CALLBACK(removeWhiteList);
CALLBACK(getWhiteList);
CALLBACK(getOnlinePlayers);
CALLBACK(getOnlinePlayerInfo);
CALLBACK(getOfflinePlayerInfo);
CALLBACK(sendTextToPlayer);
CALLBACK(executeCmd);

#undef CALLBACK

const std::unordered_map<std::string, onMessage> onMessages{
	{"hello", {hello, {"name", "introduction", "others", "xuidString"}}},
	{"consoleLog", {ConsoleLog, {"text"}}},
	{"getPerformanceUsages", {getPerformanceUsages}},
	{"getWebSocketClients", {getWebSocketClients}},
	{"addWhiteList", {addWhiteList, {"name"}}},
	{"removeWhiteList", {removeWhiteList, {}, {"name", "xuid"}}},
	{"getWhiteList", {getWhiteList}},
	{"getOnlinePlayers", {getOnlinePlayers}},
	{"getOnlinePlayerInfo", {getOnlinePlayerInfo, {"player"}}},
	{"getOfflinePlayerInfo", {getOfflinePlayerInfo, {}, {"xuid", "uuid", "realName"}}},
	{"sendTextToPlayer", {sendTextToPlayer, {"player", "text", "textType"}}},
	{"executeCmd", {executeCmd, {"cmd"}}}
};

#endif // !ONMESSAGE_H
