#ifndef MESS_H
#define MESS_H
#include "pch.h"
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

////////////////////////////////// MC //////////////////////////////////

inline void forEachPlayer(std::function<bool(Player&)> cb)
{
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

inline Certificate* getCert(Player* pl)
{
#if defined(BDS_1_16)
	return dAccess<Certificate*, 2736>(pl);
#elif defined(BDS_LATEST)
	auto ueic = SymCall(Symbol::Actor::tryGetComponent_UEIC, void*, Actor*)((Actor*)pl);
	return dAccess<Certificate*, 184>(ueic);
#else
#error "BDS version is wrong"
#endif
}
inline xuid_t getXuid(Player* pl)
{
	std::string xuid_str = SymCall(Symbol::ExtendedCertificate::getXuid,
		std::string, void*)(getCert(pl));
	return atoll(xuid_str.c_str());
}

#endif // !MESS_H

