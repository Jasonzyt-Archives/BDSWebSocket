#include "pch.h"
#include <loader/hash.h>
#include <nlohmann/json.hpp>
#include "WebSocketServer.h"
#include "Message.h"
#include "Crypt.h"
#include "LangPack.h"
#include "BDSWebSocket.h"
#include "WhiteList.h"
#include "ExtendedJson.h"
#include "cryptopp/cryptlib.h"

#define element_exists(vec,val) (find(vec.begin(), vec.end(), val) != vec.end())
#define H do_hash
#define RESPONSE Message response; response.type = msgp->type; response.id = msgp->id
#define RESPOND this->send(conn, RawMessage(response.encryptJson()).toJson())
#define CASE(x) case H(x)
#define OPTIONAL(x,y) if (msgp->data.count(x)) y = msgp->data.at(x)
#define RELEASE_MSG delete msgp
#define SET_SUCCESS response.data["success"] = true
#define ARGUMENT(x) msgp->data.at(x)
#define DATA response.data
#define CHECK_ARGS(...) \
for (auto& k : std::vector<std::string>{ __VA_ARGS__ }) { \
	if (!msgp->data.count(k)) \
		throw buildException("!WSMessage! Can't find argument '", k, "'"); \
}
#define CATCHERR \
catch (nlohmann::detail::exception e) { \
	Error() << e.what() << endl; \
	DATA["success"] = false; \
	DATA["reason"] = e.what(); \
} catch (CryptoPP::Exception e) { \
	Error() << e.what() << endl; \
	DATA["success"] = false; \
	DATA["reason"] = e.what(); \
} catch (std::exception e) { \
	Error() << e.what() << endl; \
	DATA["success"] = false; \
	DATA["reason"] = e.what(); \
} catch (...) { \
	DATA["success"] = false; \
	DATA["reason"] = "Unknown exception!"; \
}

using namespace std;
using namespace Logger;
using namespace crow;

void WebSocketServer::parseMessage(websocket::connection& conn, Message* msgp) {
	switch ((msgp->type.empty() ? H(msgp->event) : H(msgp->type)))
	{
	CASE("consoleLog"): {
		tasks.push([&]() {
			RESPONSE;
			try {
				CHECK_ARGS("text");
				Info() << ARGUMENT("text").get<string>() << endl;
				SET_SUCCESS;
			}
			CATCHERR;
			RESPOND;
			RELEASE_MSG;
		});
		break;
	}
	CASE("sendTextPlayer"): {
		tasks.push([&]() {
			RELEASE_MSG;
		});
		break;
	}
	CASE("broadcast"): {
		tasks.push([&]() {
			RELEASE_MSG;
		});
		break;
	}
	CASE("addWhitelist"): {
		tasks.push([&]() {
			RESPONSE;
			try {
				CHECK_ARGS("name");
				xuid_t xid = 0;
				bool ignores = false;
				OPTIONAL("xuid", xid);
				OPTIONAL("ignoresPlayerLimit", ignores);
				WhiteList().add(ARGUMENT("name"), xid, ignores).reload();
				SET_SUCCESS;
			}
			CATCHERR;
			RESPOND;
			RELEASE_MSG;
		});
		break;
	}
	CASE("removeWhitelist"): {
		tasks.push([&]() {
			RESPONSE;
			try {
				xuid_t xid = 0;
				string name;
				OPTIONAL("xuid", xid);
				OPTIONAL("name", name);
				if (xid > 0) WhiteList().remove(xid).reload();
				else if (!name.empty()) WhiteList().remove(name).reload();
				else throw 
					std::exception("At least one of 'name' and 'xuid' is required as a parameter");
				SET_SUCCESS;
			}
			CATCHERR;
			RESPOND;
			RELEASE_MSG;
		});
		break;
	}
	CASE("listWhitelist"): {
		tasks.push([&]() {
			RESPONSE;
			try {
				auto ls = WhiteList().list();
				for (auto& [name, xid] : ls) {
					DATA["list"].push_back(
						nlohmann::json{ {"name", name}, {"xuid", xid}});
				}
				SET_SUCCESS;
			}
			CATCHERR;
			RESPOND;
			RELEASE_MSG;
		});
		break;
	} 
	CASE("getPerformanceUsages"): {
		tasks.push([&]() {
			RESPONSE;
			try {
				// Disk Usage
				auto dinfo = GetLogicalDrives();
				int diskNum = 0;
				while (dinfo) {
					if (dinfo & 1) diskNum++;
					dinfo >>= 1;
				}
				auto len = GetLogicalDriveStringsA(0, 0);
				if (!len) throw std::exception("Error when trying getting the disk information");
				char* str = new char[(size_t)len + 10]{0};
				GetLogicalDriveStringsA(len, str);
				auto diskNames = split(string(str), '\0');
				diskNames.pop_back();
				for (auto& diskName : diskNames) {
					Info() << diskName << endl;
					DATA["disks"][diskName.substr(0, 1)] = getDiskUsage(diskName[0]);
				}
				// Memory Usage
				DATA["memory"] = getMemoryUsage();
				DATA["BDSMemory"] = getCurrentProcessMemoryUsage();
				// CPU Usage
				DATA["cpu"] = getCpuUsage();
				DATA["BDSCpu"] = getCurrentProcessCpuUsage();
				SET_SUCCESS;
			}
			CATCHERR;
			RESPOND;
			RELEASE_MSG;
		});
		break;
	}
	}
}

void WebSocketServer::run(unsigned short port) {
	if (is_running) { return; }

	CROW_ROUTE(app, "/").websocket()

		.onerror([&](websocket::connection& conn) {
			auto addr = conn.get_remote_ip();
			auto port = conn.get_remote_port();
			auto uri = addr + ':' + to_string(port);
			Warn() << bdsws->lpk->localization("ws.onerror", uri) << endl;
		})

		.onopen([&](websocket::connection& conn) {
			lock_guard<mutex> _a(mtx);
			auto addr = conn.get_remote_ip();
			auto port = conn.get_remote_port();
			auto uri = addr + ':' + to_string(port);
			clients.emplace(uri, conn);
			if (!bdsws->cfg->IPWhiteList.empty() && 
				!element_exists(bdsws->cfg->IPWhiteList, addr)) {
				Warn() << bdsws->lpk->localization("ws.notallowed", uri) << endl;
				conn.close("Your IP isn't allowed!");
				return;
			}
			Info() << bdsws->lpk->localization("ws.onopen", uri) << endl;
		})

		.onclose([&](websocket::connection& conn, const std::string& reason) {
			lock_guard<mutex> _a(mtx);
			auto addr = conn.get_remote_ip();
			auto port = conn.get_remote_port();
			auto uri = addr + ':' + to_string(port);
			clients.erase(uri);
			Info() << bdsws->lpk->localization("ws.onclose", uri, 
				(reason.empty() ? "Normal" : reason)) << endl;
		})

		.onmessage([&](websocket::connection& conn, const std::string& data, bool is_binary) {
			auto addr = conn.get_remote_ip();
			auto port = conn.get_remote_port(); 
			auto uri = addr + ':' + to_string(port);
			Info() << bdsws->lpk->localization("ws.onreceived",
				uri, (is_binary ? "[BINARY]" : data.c_str())) << endl;
			if (is_binary) return;
			try {
				RawMessage rmsg = RawMessage::fromJson(data);
				if (rmsg.encrypted) {
					if (rmsg.mode == "AES/CBC/PKCS7Padding") {
						auto ak = bdsws->getAESKey();
						auto plain = base64_aes_cbc_decrypt(rmsg.data, ak);
						Info() << "Plain Text: " << plain << endl;
						Message* msg = Message::fromJsonToPtr(plain);
						parseMessage(conn, msg);
					}
				}
				else {
					Message* msg = Message::fromJsonToPtr(rmsg.data);
					parseMessage(conn, msg);
				}
			}
			catch (nlohmann::detail::exception e) {
				Error() << e.what() << endl;
				nlohmann::json resp{ {"type", "error"}, 
					{"data", { {"reason", e.what()} }} };
				send(conn, RawMessage(resp.dump(), false).toJson());
			}
			catch (CryptoPP::Exception e) {
				Error() << e.what() << endl;
				nlohmann::json resp{ {"type", "error"}, 
					{"data", { {"reason", e.what()} }} };
				send(conn, RawMessage(resp.dump(), false).toJson());
			}
			catch (std::exception e) {
				Error() << e.what() << endl;
				nlohmann::json resp{ {"type", "error"}, 
					{"data", { {"reason", e.what()} }} };
				send(conn, RawMessage(resp.dump(), false).toJson());
			}
		});

	app.loglevel(LogLevel::Warning);

	thread th([&](unsigned short pt) {
		is_running = true;
		Info() << bdsws->lpk->localization("ws.onstart", pt) << endl;
		app.port(pt).multithreaded().run();
	}, port);
	th.detach();
}
void WebSocketServer::stop() {
	lock_guard<mutex> _a(mtx);
	if (!is_running) { return; }
	for (auto& [addr, client] : clients) {
		client.close();
	}
	app.stop();
	is_running = false;
	Info() << bdsws->lpk->localization("ws.onstop") << endl;
}

void WebSocketServer::send(const string& uri, const string& msg) {
	if (clients.count(uri)) {
		auto& conn = clients.at(uri);
		send(conn, msg);
	}
}

void WebSocketServer::send(websocket::connection& conn, const string& msg) {
	lock_guard<mutex> _a(mtx);
	conn.send_text(msg);
	auto addr = conn.get_remote_ip();
	auto port = conn.get_remote_port();
	auto uri = addr + ':' + to_string(port);
	Info() << bdsws->lpk->localization("ws.onsent", uri, msg.c_str()) << endl;
}

void WebSocketServer::sendAll(const string& msg) {
	for (auto& client : clients) {
		send(client.first, msg);
	}
}

bool WebSocketServer::isRunning() {
	return is_running;
}

void WebSocketServer::processTask() {
	if (!tasks.empty()) {
		tasks.front()();
		tasks.pop();
	}
}