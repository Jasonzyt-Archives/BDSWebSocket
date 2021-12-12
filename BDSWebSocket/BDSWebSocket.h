#ifndef BDSWEBSOCKET_H
#define BDSWEBSOCKET_H
#include "pch.h"
#include "Config.h"
#include "WebSocketServer.h"
#include "System.h"

struct AESKey;
class LangPack;
class Level;
class Player;
class Minecraft;
class NetworkIdentifier;
class OfflineStorage;

struct RakPeer_t;
struct PropertiesSettings;

class BDSWebSocket {

public:

	// Plugin
	std::unique_ptr<Config> cfg;
	std::unique_ptr<LangPack> lpk;
	std::unique_ptr<WebSocketServer> ws;
	std::unique_ptr<OfflineStorage> ols;
	GlobalCpuUsage gcu;
	CurrentProcessCpuUsage cpcu;
	// MC
	Level* level = nullptr;
	Minecraft* mc = nullptr;
	void* wlfile = nullptr;
	uint16_t ipv4Port = 0;
	uint16_t ipv6Port = 0;
	PropertiesSettings* properties = nullptr;
	RakPeer_t* rakpeer = nullptr;
	std::unordered_map<Player*, NetworkIdentifier*> nid;

	BDSWebSocket(Config* cfg);

	~BDSWebSocket();

	AESKey getAESKey();

};

#endif // !BDSWEBSOCKET_H
