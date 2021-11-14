#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H
#include "pch.h"
#include <crow/crow_all.h>

class Message;
class WebSocketServer {
	
	bool is_running = false;
	std::queue<std::function<void()>> tasks;
	std::unordered_map<std::string, crow::websocket::connection&> clients;
	std::mutex mtx;
	crow::SimpleApp app;

	void parseMessage(crow::websocket::connection& conn, Message* msgp);

public:

	void run(unsigned short port);
	void stop();

	void send(const std::string& ip, const std::string& msg);
	void send(crow::websocket::connection& conn, const std::string& msg);
	void sendAll(const std::string& msg);

	bool isRunning();

	void processTask();
};

#endif // !WEBSOCKETSERVER_H
