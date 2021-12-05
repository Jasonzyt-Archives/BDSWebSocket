#ifndef MESS_H
#define MESS_H
#include "pch.h"
#include "Symbols.h"
#include "BDSWebSocket.h"
#include <mc/Player.h>
#include <mc/Certificate.h>
#include <mc/Level.h>

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

//------------------------------ Player ------------------------------//
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

