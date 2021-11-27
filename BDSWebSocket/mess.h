#ifndef MESS_H
#define MESS_H
#include "pch.h"
#include "BDSWebSocket.h"
#include <Psapi.h>
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

////////////////////////////////// SYSTEM //////////////////////////////////

struct DiskUsage {
	uint32_t type;
	size_t free;   // Free bytes
	size_t avfree; // Available free bytes
	size_t total;  // Total bytes
};

struct MemoryUsage {
	size_t physicalUsed;  // Used physical memory (byte)
	size_t physicalTotal; // Total physucal memory (byte)
	size_t virtualUsed;   // Used virtual memory (byte)
	size_t virtualTotal;  // Total virtual memory (byte)
	uint32_t rate;        // Memory usage rate(0-100)
};

struct CpuUsage {
	uint32_t core; // CPU cores
	double rate;   // CPU usage rate
};

inline DiskUsage getDiskUsage(char s) {
	size_t avfree = 0; // Available Free Bytes
	size_t total = 0;  // Total Bytes
	size_t free = 0;   // Free Bytes
	const char dname[] = { s, ':', '\0'};
	auto dtype = GetDriveTypeA(dname);
	auto res = GetDiskFreeSpaceExA(dname,
		(ULARGE_INTEGER*)&avfree,
		(ULARGE_INTEGER*)&total,
		(ULARGE_INTEGER*)&free);
	if (res) {
		return { dtype, free, avfree, total };
	}
	else throw buildException("Error when trying getting the disk information of '", dname, "'");
}

inline MemoryUsage getMemoryUsage() {
	MemoryUsage result;
	MEMORYSTATUSEX ms;
	auto res = GlobalMemoryStatusEx(&ms);
	if (res) {
		result.physicalUsed = ms.ullTotalPhys - ms.ullAvailPhys;
		result.physicalTotal = ms.ullTotalPhys;
		result.virtualUsed = ms.ullTotalVirtual - ms.ullAvailVirtual;
		result.virtualTotal = ms.ullTotalVirtual;
		result.rate = ms.dwMemoryLoad;
		return result;
	}
	else throw buildException("Error when trying getting the memory information");
}

inline MemoryUsage getCurrentProcessMemoryUsage() {
	MemoryUsage result;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	auto res = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	if (res) {
		result.physicalUsed = pmc.WorkingSetSize;
		result.physicalTotal = pmc.PeakWorkingSetSize;
		result.virtualUsed = pmc.PagefileUsage;
		result.virtualTotal = pmc.PeakPagefileUsage;
		result.rate = pmc.PrivateUsage;
		return result;
	}
	else throw buildException("Error when trying getting the memory information");
}

inline CpuUsage getCpuUsage() {
	CpuUsage result;
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	auto res = GetSystemTimes(&ftime, &fsys, &fuser);
	if (res) {
		now.LowPart = ftime.dwLowDateTime;
		now.HighPart = ftime.dwHighDateTime;
		sys.LowPart = fsys.dwLowDateTime;
		sys.HighPart = fsys.dwHighDateTime;
		user.LowPart = fuser.dwLowDateTime;
		user.HighPart = fuser.dwHighDateTime;
		result.rate = (user.QuadPart - sys.QuadPart) / (now.QuadPart - sys.QuadPart);
		result.core = std::thread::hardware_concurrency();
		return result;
	}
	else throw buildException("Error when trying getting the CPU information");
}

inline CpuUsage getCurrentProcessCpuUsage() {
	CpuUsage result;
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	auto res = GetProcessTimes(GetCurrentProcess(), &ftime, &fsys, &fuser, &ftime);
	if (res) {
		now.LowPart = ftime.dwLowDateTime;
		now.HighPart = ftime.dwHighDateTime;
		sys.LowPart = fsys.dwLowDateTime;
		sys.HighPart = fsys.dwHighDateTime;
		user.LowPart = fuser.dwLowDateTime;
		user.HighPart = fuser.dwHighDateTime;
		result.rate = (user.QuadPart - sys.QuadPart) / (now.QuadPart - sys.QuadPart);
		return result;
	}
	else throw buildException("Error when trying getting the CPU information");
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
inline std::string getClientAddress(NetworkIdentifier* nid) {
	return SymCall(Symbol::NetworkIdentifier::getAddress, 
		std::string, NetworkIdentifier*)(nid);
}

inline ServerPlayer* getServerPlayer(ServerNetworkHandler* thiz, 
	NetworkIdentifier* ni, unsigned char sid) {
	return SymCall(Symbol::ServerNetworkHandler::_getServerPlayer, 
		ServerPlayer*, ServerNetworkHandler*, NetworkIdentifier*, unsigned char)(thiz, ni, sid);
}

#endif // !MESS_H

