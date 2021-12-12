#ifndef MESS_H
#define MESS_H
#include "pch.h"
#include "Symbols.h"
#include "BDSWebSocket.h"
#include <mc/Player.h>
#include <mc/Certificate.h>
#include <mc/Level.h>
#include "mc/Packet.h"
#include <LLSDK/api/types/types.h>

////////////////////////////////// UTILS //////////////////////////////////

inline std::string randomString(int len) {
	const std::string letters = 
		"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
	std::string result;
	for (int i = 0; i < len; i++) {
		auto r = rand();
		result += letters[(r % 62)];
	}
	return result;
}

template<typename ... Args>
inline std::string format(const std::string& format, Args... args)
{
	int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
	if (size <= 0)
		return "";
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1);
}

inline std::vector<std::string> split(const std::string& str, char delim = ' ') {
	std::vector<std::string> result;
	std::istringstream iss(str);
	std::string line;
	while (std::getline(iss, line, delim)) {
		result.push_back(line);
	}
	return result;
}

inline std::string SecondToStr(time_t sec) {
	auto d = sec / (60 * 60 * 24);
	sec %= 60 * 60 * 24;
	auto hr = sec / (60 * 60);
	sec %= 60 * 60;
	auto mi = sec / 60;
	sec %= 60;
	return format("%d d %d h %d m %d s", d, hr, mi, sec);
}

template<typename Stream, typename T>
inline void append(Stream& s, T v) {
	s << v;
}

template<typename Stream, typename T, typename ... Args>
inline void append(Stream& s, T v, Args ... args) {
	s << v;
	append(s, args...);
}

template<typename ... Args>
inline std::exception buildException(Args ... args) {
	std::ostringstream oss;
	oss << "BDSWebSocket: ";
	append(oss, args...);
	return std::exception(oss.str().c_str());
}

////////////////////////////////// MC //////////////////////////////////

namespace mce {
	class UUID {
		char filler[16];
	public:
		inline std::string asString() {
			std::string result;
			SymCall(Symbol::mce::UUID::asString, std::string*, UUID*, std::string*)
				(this, &result);
			return result;
		}
	};
}

enum ActorDamageCause : int
{
	Override = 0x0,          // 
	Contact = 0x1,           //
	EntityAttack = 0x2,      // 实体攻击
	Projectile = 0x3,        // 弹射物
	Suffocation = 0x4,       // 窒息(卡方块)
	Fall = 0x5,              // 摔落
	Fire = 0x6,              // inFire(在火焰中)
	FireTick = 0x7,          // onFire(自身着火)
	Lava = 0x8,              // 岩浆
	Drowning = 0x9,          // 溺水
	BlockExplosion = 0x0A,   // 方块爆炸
	EntityExplosion = 0x0B,  // 实体爆炸
	Void = 0x0C,             // 虚空
	Suicide = 0x0D,          // 自杀(?)
	Magic = 0x0E,            // 魔法(药水)
	Wither = 0x0F,           // 凋零效果
	Starve = 0x10,           // 饥饿
	Anvil = 0x11,            // 铁砧
	Thorns = 0x12,           // 荆棘
	FallingBlock = 0x13,     // 坠落方块砸中
	Piston = 0x14,           // 中毒
	FlyIntoWall = 0x15,      // 飞进墙里(鞘翅撞死)
	Magma = 0x16,            // 岩浆块
	Fireworks = 0x17,        // 烟花爆炸
	Lightning = 0x18,        // 闪电
	Charging = 0x19,         // 
	Temperature = 0x1A,      // 
};

//------------------------------ Actor ------------------------------//

inline Actor* fetchActor(ActorUniqueID uid) {
	return SymCall(Symbol::Level::fetchActor, 
		Actor*, Level*, ActorUniqueID, bool)(bdsws->level, uid, false);
}

//------------------------------ Player ------------------------------//

// Note: return false in cb to stop forEach
inline void forEachPlayer(std::function<bool(Player&)> cb) {
	SymCall(Symbol::Level::forEachPlayer,
		void, Level*, std::function<bool(Player&)>)(bdsws->level, cb);
}
inline std::vector<Player*> getAllPlayers() {
	std::vector<Player*> player_list;
	forEachPlayer([&](Player& pl) {
		Player* player = &pl;
		if (!player) return false;
		player_list.push_back(player);
		return true;
		});
	return player_list;
}

inline Certificate* getCert(Player* pl) {
#if defined(BDS_1_16)
	return dAccess<Certificate*, 2736>(pl);
#elif defined(BDS_LATEST)
	auto ueic = SymCall(Symbol::Actor::tryGetComponent_UEIC, void*, Actor*)((Actor*)pl);
	return dAccess<Certificate*, 184>(ueic);
#else
#error "BDS version is wrong"
#endif
}

inline xuid_t getXuid(Player* pl) {
	std::string xuid_str = SymCall(Symbol::ExtendedCertificate::getXuid,
		std::string, void*)(getCert(pl));
	return atoll(xuid_str.c_str());
}
inline std::string getRealName(Player* pl) {
	return SymCall(Symbol::ExtendedCertificate::getIdentityName,
		std::string, void*)(getCert(pl));
}
inline mce::UUID getUuid(Player* pl) {
#if defined(BDS_1_16)
	return dAccess<mce::UUID, 2720>(pl);
#endif
}

inline Player* getPlayerByXuid(xuid_t xid) {
	Player* result = nullptr;
	forEachPlayer([&](Player& pl) {
		if (getXuid(&pl) == xid) {
			result = &pl;
			return false;
		}
		return true;
	});
	return result;
}
inline Player* getPlayerByUuid(const std::string& uuid) {
	Player* result = nullptr;
	forEachPlayer([&](Player& pl) {
		if (getUuid(&pl).asString() == uuid) {
			result = &pl;
			return false;
		}
		return true;
	});
	return result;
}
inline Player* getPlayerByUniqueID(ActorUniqueID uid) {
	Player* result = nullptr;
	/*forEachPlayer([&](Player& pl) {
		if (pl.getUniqueID().id == uid.id) {
			result = &pl;
			return false;
		}
		return true;
	});*/
	result = (Player*)fetchActor(uid);
	return result;
}
inline Player* getPlayerByName(const std::string& name) {
	Player* result = nullptr;
	forEachPlayer([&](Player& pl) {
		if (pl.getNameTag() == name) {
			result = &pl;
			return false;
		}
		return true;
	});
	return result;
}
inline Player* getPlayerByRealName(const std::string& realName) {
	Player* result = nullptr;
	forEachPlayer([&](Player& pl) {
		if (getRealName(&pl) == realName) {
			result = &pl;
			return false;
		}
		return true;
	});
	return result;
}

inline Packet* createPacket(int id);
inline void sendText(Player* pl, const std::string& text, TextType tp) {
	auto pkt = createPacket(9);
	std::string srcName = "Server";
	dAccess<TextType, 32>(pkt) = tp;
	dAccess<std::string*, 40>(pkt) = &srcName;
	dAccess<std::string*, 80>(pkt) = const_cast<std::string*>(&text);
	pl->sendNetworkPacket(*(Packet*)pkt);
}

//------------------------------ Network -----------------------------//

inline std::string getClientAddress(NetworkIdentifier* nid) {
	auto addr = bdsws->rakpeer->getAdr(*nid).toString();
	//std::replace(addr.begin(), addr.end(), '|', ':');
	return addr;
}
inline NetworkPeer* getNetworkPeer(NetworkIdentifier* nid) {
	return SymCall(Symbol::NetworkHandler::getPeerForUser,
		NetworkPeer*, ServerNetworkHandler*, NetworkIdentifier*)
		(bdsws->mc->getServerNetworkHandler(), nid);
}
inline ServerPlayer* getServerPlayer(ServerNetworkHandler* handler, 
	NetworkIdentifier* nid, unsigned char sid) {
	return SymCall(Symbol::ServerNetworkHandler::_getServerPlayer, 
		ServerPlayer*, ServerNetworkHandler*, NetworkIdentifier*, unsigned char)
		(handler, nid, sid);
}
inline int getAvgPingOfPlayer(NetworkIdentifier* nid) {
	auto np = getNetworkPeer(nid);
	return np->getNetworkStatus().avgping;
}
inline double getAvgPacketLossOfPlayer(NetworkIdentifier* nid) {
	auto np = getNetworkPeer(nid);
	return np->getNetworkStatus().avgpacketloss;
}
inline int getPingOfPlayer(NetworkIdentifier* nid) {
	auto np = getNetworkPeer(nid);
	return np->getNetworkStatus().ping;
}
inline double getPacketLossOfPlayer(NetworkIdentifier* nid) {
	auto np = getNetworkPeer(nid);
	return np->getNetworkStatus().packetloss;
}

inline Packet* createPacket(int id) {
	Packet* pkt = nullptr;
	SymCall(Symbol::MinecraftPackets::creatPacket, void*, Packet**, int)(&pkt, id);
	return pkt;
}

//------------------------- ConnectionRequest -------------------------//
inline std::string getDeviceId(void* cr) {
	return SymCall(Symbol::ConnectionRequest::getDeviceId, std::string, void*)(cr);
}
inline int getDeviceOS(void* cr) {
	//auto json = SymCall(Symbol::ConnectionRequest::getData,
	//	void*, void*, const std::string&)(cr, "DeviceOS");
	return 0;
}

#endif // !MESS_H
