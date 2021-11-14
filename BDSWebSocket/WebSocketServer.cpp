#include "pch.h"
#include <loader/hash.h>
#include <nlohmann/json.hpp>
#include "WebSocketServer.h"
#include "Message.h"
#include "Crypt.h"
#include "LangPack.h"
#include "BDSWebSocket.h"

#define element_exists(vec,val) (find(vec.begin(), vec.end(), val) != vec.end())
#define H do_hash

using namespace std;
using namespace Logger;
using namespace crow;

void WebSocketServer::parseMessage(const Message& msg) {
	auto msgp = new Message(msg);
	switch ((msg.type.empty() ? H(msg.event) : H(msg.type)))
	{
	case H("consoleLog"): {
		exec_queue.push([&]() {
			Info() << msgp->data["text"].get<string>() << endl;
			delete msgp;
		});
		break;
	}
	case H("sendTextPlayer"): {
		exec_queue.push([&]() {
			delete msgp;
		});
		break;
	}
	case H("broadcast"): {
		exec_queue.push([&]() {
			delete msgp;
		});
		break;
	}
	default:
		break;
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
					if (rmsg.mode == "AES/CBC/PKCS5Padding") {
						auto ak = bdsws->getAESKey();
						auto plain = base64_aes_cbc_decrypt(rmsg.data, ak);
						Message msg = Message::fromJson(plain);
						parseMessage(msg);
					}
				}
				else {
					Message msg = Message::fromJson(rmsg.data);
					parseMessage(msg);
				}
			}
			catch (std::exception e) {
				Error() << e.what() << endl;
			}
		});

	app.loglevel(LogLevel::Warning);

	thread th([&](unsigned short pt) {
		app.port(pt).multithreaded().run();
		is_running = true;
		Info() << bdsws->lpk->localization("ws.onstart", pt) << endl;
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
	lock_guard<mutex> _a(mtx);
	if (clients.count(uri)) {
		auto& conn = clients.at(uri);
		conn.send_text(msg);
		Info() << bdsws->lpk->localization("ws.onsent", uri, msg) << endl;
	}
}

void WebSocketServer::send(websocket::connection& conn, const string& msg) {
	lock_guard<mutex> _a(mtx);
	conn.send_text(msg);
	auto addr = conn.get_remote_ip();
	auto port = conn.get_remote_port();
	auto uri = addr + ':' + to_string(port);
	Info() << bdsws->lpk->localization("ws.onsent", uri, msg) << endl;
}

void WebSocketServer::sendAll(const string& msg) {
	lock_guard<mutex> _a(mtx);
	for (auto& client : clients) {
		send(client.first, msg);
	}
}

bool WebSocketServer::isRunning() {
	return is_running;
}

void WebSocketServer::processQueue() {
	if (!exec_queue.empty()) {
		exec_queue.front()();
		exec_queue.pop();
	}
}