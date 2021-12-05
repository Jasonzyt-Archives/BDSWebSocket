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

CALLBACK(ConsoleLog);
CALLBACK(getPerformanceUsages);

#undef CALLBACK

const std::unordered_map<std::string, onMessage> onMessages{
	{"consoleLog", {ConsoleLog, {"text"}, {}}},
	{"getPerformanceUsages", {getPerformanceUsages, {}, {}}},
};

#endif // !ONMESSAGE_H
