#ifndef BDSWEBSOCKET_H
#define BDSWEBSOCKET_H
#include "pch.h"
#include "Config.h"
#include "WebSocketServer.h"

struct AESKey;
class LangPack;
class Level;
class Player;
class Minecraft;
class PropertiesSettings;
class NetworkIdentifier;

class BDSWebSocket {

public:

	// Plugin
	std::unique_ptr<Config> cfg;
	std::unique_ptr<LangPack> lpk;
	std::unique_ptr<WebSocketServer> ws;
	std::fstream logfile;
	// MC
	Level* level = nullptr;
	Minecraft* mc = nullptr;
	void* wlfile = nullptr;
	uint16_t ipv4Port = 0;
	uint16_t ipv6Port = 0;
	PropertiesSettings* properties = nullptr;
	std::unordered_map<Player*, NetworkIdentifier*> nid;

	BDSWebSocket(Config* cfg);

	~BDSWebSocket();

	AESKey getAESKey();

};

#endif // !BDSWEBSOCKET_H
