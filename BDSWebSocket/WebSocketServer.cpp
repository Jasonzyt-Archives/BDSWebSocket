#include <unordered_set>
#include <mutex>
#include <queue>
#include <loader/hash.h>
#include <nlohmann/json.hpp>
#include "Message.h"
#include "Crypt.h"
#include "Logger.h"

#define CROW_MAIN
#include "crow/crow_all.h"

#define H do_hash

using namespace std;

queue<function<void(const Message&)>> exec;

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

void runWebSocketServer(unsigned short port) {
	crow::SimpleApp app;
	std::unordered_map<std::string, crow::websocket::connection&> clients;
	std::mutex mtx;

	CROW_ROUTE(app, "/").websocket()
		.onerror([&](crow::websocket::connection& conn) {
			
			})
		.onopen([&](crow::websocket::connection& conn) {
				lock_guard<mutex> _a(mtx);
				clients.emplace(conn.get_remote_ip(), conn);
			})
		.onclose([&](crow::websocket::connection& conn, const std::string& reason) {
				lock_guard<mutex> _a(mtx);
				clients.erase(conn.get_remote_ip());
			})
		.onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
				if (is_binary) return;
				RawMessage msg = RawMessage::fromJson(data);
				try {
					if (msg.encrypted) {
						// if (msg.mode == "AES/CBC/PKCS5Padding") {

						// }
					}
				}
				catch (exception e) {

				}
			});

	app.port(port).multithreaded().run();
}