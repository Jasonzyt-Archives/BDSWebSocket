#include "pch.h"
#include <loader/hash.h>
#include <nlohmann/json.hpp>
#include "WebSocketServer.h"
#include "Message.h"
#include "Crypt.h"
#include "BDSWebSocket.h"

#define element_exists(vec,val) (find(vec.begin(), vec.end(), val) != vec.end())
#define H do_hash

using namespace std;
using namespace Logger;

void parseMessage(const Message& msg) {
	switch ((msg.type.empty() ? H(msg.event) : H(msg.type)))
	{
	case H(""): {
		break;
	}
	default:
		break;
	}
}

void WebSocketServer::run(unsigned short port) {
	if (is_running) { return; }

	CROW_ROUTE(app, "/").websocket()

		.onerror([&](crow::websocket::connection& conn) {
				auto addr = conn.get_remote_ip();
				auto port = conn.get_remote_port();
				Warn() << bdsws->lpk->localization("ws.onerror", addr, port) << endl;
			})

		.onopen([&](crow::websocket::connection& conn) {
				lock_guard<mutex> _a(mtx);
				auto addr = conn.get_remote_ip();
				auto port = conn.get_remote_port();
				clients.emplace(addr, conn);
				if (!bdsws->cfg->IPWhiteList.empty() && 
					!element_exists(bdsws->cfg->IPWhiteList,addr)) {
					Warn() << bdsws->lpk->localization("ws.notallowed", addr, port) << endl;
					conn.close("Your IP isn't allowed!");
					return;
				}
				Info() << bdsws->lpk->localization("ws.onopen", addr, port) << endl;
			})

		.onclose([&](crow::websocket::connection& conn, const std::string& reason) {
				lock_guard<mutex> _a(mtx);
				auto addr = conn.get_remote_ip();
				auto port = conn.get_remote_port();
				clients.erase(conn.get_remote_ip());
				Info() << bdsws->lpk->localization("ws.onclose", addr, port, reason) << endl;
			})

		.onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
				auto addr = conn.get_remote_ip();
				auto port = conn.get_remote_port(); 
				Info() << bdsws->lpk->localization("onreceived",
					addr, port, (is_binary ? "[BINARY]" : data)) << endl;
				if (is_binary) return;
				try {
					RawMessage msg = RawMessage::fromJson(data);
					if (msg.encrypted) {
						// if (msg.mode == "AES/CBC/PKCS5Padding") {

						// }
					}
				}
				catch (exception e) {

				}
			});

	thread th([&]() {	
		app.port(port).multithreaded().run();
		is_running = true;
	});
	th.detach();
}
void WebSocketServer::stop() {
	if (!is_running) { return; }
	app.stop();
	is_running = false;
}

void WebSocketServer::send(const string& ip, const string& msg) {
	if (clients.count(ip)) {
		auto& conn = clients.at(ip);
		auto addr = conn.get_remote_ip();
		auto port = conn.get_remote_port();
		conn.send_text(msg);
		Info() << bdsws->lpk->localization("ws.onsent", addr, port, msg) << endl;
	}
}

void WebSocketServer::sendAll(const string& msg) {
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