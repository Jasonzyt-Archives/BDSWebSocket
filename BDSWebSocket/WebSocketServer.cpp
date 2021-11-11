#include "pch.h"
#include <loader/hash.h>
#include <nlohmann/json.hpp>
#include <crow/crow_all.h>
#include "Message.h"
#include "Crypt.h"
#include "BDSWebSocket.h"

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


crow::SimpleApp app;

void runWebSocketServer(unsigned short port) {

	CROW_ROUTE(app, "/").websocket()

		.onerror([&](crow::websocket::connection& conn) {
				auto addr = conn.get_remote_ip();
				auto port = conn.get_remote_port();
				Warn() << bdsws->lpk->localization("ws.onerror", addr, port) << endl;
			})

		.onopen([&](crow::websocket::connection& conn) {
				lock_guard<mutex> _a(bdsws->ws_mtx);
				auto addr = conn.get_remote_ip();
				auto port = conn.get_remote_port();
				bdsws->clients.emplace(addr, conn);
				//if (bdsws->cfg->IPWhiteList);
				Warn() << bdsws->lpk->localization("ws.onopen", addr, port) << endl;
			})

		.onclose([&](crow::websocket::connection& conn, const std::string& reason) {
				lock_guard<mutex> _a(bdsws->ws_mtx);
				auto addr = conn.get_remote_ip();
				auto port = conn.get_remote_port();
				bdsws->clients.erase(conn.get_remote_ip());
				Warn() << bdsws->lpk->localization("ws.onclose", addr, port) << endl;
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
	});
	th.detach();
}

void stopWebSocketServer() {
	app.stop();
}