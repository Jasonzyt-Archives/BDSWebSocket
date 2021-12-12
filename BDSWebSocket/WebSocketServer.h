#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H
#include "pch.h"
#include <crow/crow_all.h>
#include <nlohmann/json.hpp>

class Message;

struct ClientSettings {
	std::string name; // Project name
	std::string intro; // Project introduction
	nlohmann::json others;

	// JSON中的Xuid是否通过字符串传递(适配一些没有完整ULL的语言,如JavaScript)
	// 靠, JS这什么傻逼设计, 反人类
	// Tips: JS的Number类型有个最大值(安全值),即2^53
	bool xuidString = false;
};

class WebSocketServer {
	
	bool is_running = false;
	std::queue<std::function<void()>> tasks;
	std::unordered_map<std::string, crow::websocket::connection&> clients;
	std::unordered_map<std::string, ClientSettings> settings;
	std::mutex mtx;
	crow::SimpleApp app;
	time_t start = 0;

	void handle(crow::websocket::connection& conn, Message* msgp);

public:

	void run(unsigned short port);
	void stop();

	void send(const std::string& addr, const std::string& msg);
	void send(crow::websocket::connection& conn, Message msg);
	void send(crow::websocket::connection& conn, const std::string& msg);
	void sendAll(const std::string& msg);
	void sendEvent(Message msg);

	std::unordered_map<std::string, crow::websocket::connection&> getClients();
	ClientSettings& getClientSettings(const std::string& addr);

	bool isRunning();

	void processTask();
};

#endif // !WEBSOCKETSERVER_H
