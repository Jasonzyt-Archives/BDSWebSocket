#include "pch.h"
#include "Message.h"
#include "System.h"
#include "Mess.h"
#include "BDSWebSocket.h"
#include "ExtendedJson.h"
#include <crow/crow_all.h>

using namespace std;
using namespace Logger;
using WSConn = crow::websocket::connection;

#define OPTION(x,y) if (msgp->data.count(x)) y = msgp->data.at(x)
#define ARGUMENT(x) msgp->data.at(x)

nlohmann::json ConsoleLog(WSConn& conn, Message* msgp) {
	auto addr = conn.get_remote_ip() + ':' + to_string(conn.get_remote_port());
	Info() << "WS Client " << addr << ": " << ARGUMENT("text").get<string>() << endl;
	return nlohmann::json::object();
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