#include "WebSocketServer.h"
#include <nlohmann/json.hpp>
#include "onMessage.h"
#include "WebSocketLogger.h"
#include "Message.h"
#include "Crypt.h"
#include "LangPack.h"
#include "BDSWebSocket.h"
#include "cryptopp/cryptlib.h"

#define element_exists(vec,val) (find(vec.begin(), vec.end(), val) != vec.end())
#define RESPONSE Message response; response.type = msgp->type; response.id = msgp->id
#define RESPOND this->send(conn, response)
#define RELEASE_MSG delete msgp
#define SET_SUCCESS response.data["success"] = true
#define DATA response.data

#define CATCH_ERR \
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

void convertXuidFromULLToString(nlohmann::json& json) {
	for (auto i = json.begin(); i != json.end(); i++) {
		if (json.is_object()) {
			if (i.key() == "xuid") {
				auto v = i.value();
				if (!v.is_string())
					i.value() = to_string(v.get<xuid_t>());
			}
			if (i.value().is_object()) {
				convertXuidFromULLToString(i.value());
			}
		}
	}
}

void WebSocketServer::handle(websocket::connection& conn, Message* msgp) {
	auto req = (msgp->type.empty() ? msgp->event : msgp->type);
	if (onMessages.count(req)) {
		auto onMsg = onMessages.at(req);
		if (!onMsg.args.empty()) {
			for (auto& k : onMsg.args) {
				if (!msgp->data.count(k)) {
					RESPONSE;
					DATA["success"] = false;
					DATA["reason"] = "WSMessage: Couldn't find argument '" + k + "'";
					RESPOND;
					RELEASE_MSG;
					return;
				}
			}
		}
		if (!onMsg.option.empty()) {
			bool flag = false;
			string options;
			for (auto& k : onMsg.option) {
				if (msgp->data.count(k)) {
					flag = true;
					break;
				}
				options += k + ", ";
			}
			if (!flag) {
				RESPONSE;
				DATA["success"] = false;
				DATA["reason"] = "WSMessage: Could't find optional arguments (Need any of '" +
					options.substr(0, options.size() - 2) + ")'";
				RESPOND;
				RELEASE_MSG;
				return;
			}
		}
		tasks.push([&]() {
			unique_ptr<Message> msg(std::move(msgp));
			RESPONSE;
			try {
				auto req = (msg->type.empty() ? msg->event : msg->type);
				auto resp_data = onMessages.at(req).cb(conn, msg.get());
				DATA = resp_data;
				SET_SUCCESS; // To set error, plz throw an exception in callback
			}
			CATCH_ERR;
			RESPOND;
		});
	}
	else {
		RESPONSE;
		DATA["success"] = false;
		DATA["reason"] = "Unknown request/event '" + req + "'";
		RESPOND;
		RELEASE_MSG;
	}
}

void WebSocketServer::run(unsigned short port) {
	if (is_running) { return; }

	CROW_ROUTE(app, "/mc").websocket()

		.onerror([&](websocket::connection& conn) {
			auto ip = conn.get_remote_ip();
			auto port = conn.get_remote_port();
			auto addr = ip + ':' + to_string(port);
			Warn() << bdsws->lpk->localization("ws.onerror", addr) << endl;
			WSLogger(WARN) << "Client Error: " << addr << endl;
		})

		.onopen([&](websocket::connection& conn) {
			lock_guard<mutex> _a(mtx);
			auto ip = conn.get_remote_ip();
			auto port = conn.get_remote_port();
			auto addr = ip + ':' + to_string(port);
			clients.emplace(addr, conn);
			settings.emplace(addr, ClientSettings());
			if (!bdsws->cfg->IPWhiteList.empty() && 
				!element_exists(bdsws->cfg->IPWhiteList, addr)) {
				Warn() << bdsws->lpk->localization("ws.notallowed", addr) << endl;
				WSLogger(WARN) << "WebSocket client '" << addr <<
					"' tried connecting server, but it isn't in IPWhiteList!"
					"Disconnecting..." << endl;
				conn.close("Your IP isn't allowed!");
				return;
			}
			Info() << bdsws->lpk->localization("ws.onopen", addr) << endl;
			WSLogger(INFO) << "WebSocket client connected! Address: " << addr << endl;
			WSLogger(INFO) << "Now there is currently " << clients.size() <<
				"client(s) staying connected to the server" << endl;
		})

		.onclose([&](websocket::connection& conn, const std::string& reason) {
			lock_guard<mutex> _a(mtx);
			auto ip = conn.get_remote_ip();
			auto port = conn.get_remote_port();
			auto addr = ip + ':' + to_string(port);
			clients.erase(addr);
			settings.erase(addr);
			Info() << bdsws->lpk->localization("ws.onclose", addr, 
				(reason.empty() ? "Normal" : reason)) << endl;
			WSLogger(INFO) << "WebSocket client disconnected! Address: " << addr << endl;
			WSLogger(INFO) << "Now there is currently" << clients.size() <<
				"client(s) staying connected to the server" << endl;
		})

		.onmessage([&](websocket::connection& conn, const std::string& data, bool is_binary) {
			auto ip = conn.get_remote_ip();
			auto port = conn.get_remote_port(); 
			auto addr = ip + ':' + to_string(port);
#if !defined(RELEASE)
			Info() << bdsws->lpk->localization("ws.onreceived",
				addr, (is_binary ? "[BINARY]" : data.c_str())) << endl;
#endif
			WSLogger(INFO) << "Received from " << addr << " <- " 
				<< (is_binary ? "[BINARY]" : data) << endl;
			if (is_binary) return;
			try {
				RawMessage rmsg = RawMessage::fromJson(data);
				if (rmsg.encrypted) {
					if (rmsg.mode == "AES/CBC/PKCS7Padding") {
						auto ak = bdsws->getAESKey();
						auto plain = base64_aes_cbc_decrypt(rmsg.data, ak);
#if !defined(RELEASE)
						Info() << "Plain text: " << plain << endl;
						WSLogger(INFO) << "Plain text: " << plain << endl;
#endif
						Message* msg = Message::fromJsonToPtr(plain);
						handle(conn, msg);
					}
				}
				else {
					Message* msg = Message::fromJsonToPtr(rmsg.data);
					handle(conn, msg);
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
		start = time(NULL);
		Info() << bdsws->lpk->localization("ws.onstart", pt) << endl;
		WSLogger::init();
		WSLogger(INFO) << "WebSocket server started! Port: " << pt << endl;
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
	auto otime = SecondToStr(time(NULL) - start);
	is_running = false;
	start = 0;
	Info() << bdsws->lpk->localization("ws.onstop", otime.c_str()) << endl;
	WSLogger(INFO) << "WebSocket server stopped! (Opening time: "
		<< otime << ')' << endl;
	WSLogger::uninit();
}

void WebSocketServer::send(const string& addr, const string& msg) {
	if (clients.count(addr)) {
		auto& conn = clients.at(addr);
		send(conn, msg);
	}
}

void WebSocketServer::send(websocket::connection& conn, Message msg) {
	auto ip = conn.get_remote_ip();
	auto port = conn.get_remote_port();
	auto addr = ip + ':' + to_string(port);
	if (settings[addr].xuidString) {
		convertXuidFromULLToString(msg.data);
	}
	auto content = RawMessage(msg.encryptJson()).toJson();
	send(conn, content);
}

void WebSocketServer::send(websocket::connection& conn, const string& msg) {
	lock_guard<mutex> _a(mtx);
	conn.send_text(msg);
	auto ip = conn.get_remote_ip();
	auto port = conn.get_remote_port();
	auto addr = ip + ':' + to_string(port);
	Info() << bdsws->lpk->localization("ws.onsent", addr, msg.c_str()) << endl;
	WSLogger(INFO) << "Sent to " << addr << " -> " << msg << endl;
}

void WebSocketServer::sendAll(const string& msg) {
	for (auto& client : clients) {
		send(client.first, msg);
	}
}

void WebSocketServer::sendEvent(Message msg) {
	for (auto& client : clients) {
		send(client.second, msg);
	}
}
unordered_map<string, websocket::connection&> WebSocketServer::getClients() {
	return clients;
}
ClientSettings& WebSocketServer::getClientSettings(const string& addr) {
	return settings.at(addr);
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