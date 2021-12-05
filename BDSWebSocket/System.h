#ifndef SYSTEM_H
#define SYSTEM_H
#include "pch.h"
#include <pdh.h>
#include <pdhmsg.h>
#include <Psapi.h>
#pragma comment(lib, "pdh.lib")

template<typename ... Args>
std::exception buildException(Args ... args);

class GlobalCpuUsage {

	PDH_HQUERY cpuQueryTotal;
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	PDH_HCOUNTER cpu;

public:

	~GlobalCpuUsage() {
		PdhCloseQuery(cpuQuery);
		PdhCloseQuery(cpuQueryTotal);
	}

	inline void init() {
		PdhOpenQuery(NULL, NULL, &cpuQuery);
		PdhAddEnglishCounter(cpuQuery, L"\\Processor(*)\\% Processor Time", NULL, &cpu);
		PdhCollectQueryData(cpuQuery);
		
		PdhOpenQuery(NULL, NULL, &cpuQueryTotal);
		// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
		PdhAddEnglishCounter(cpuQueryTotal, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
		PdhCollectQueryData(cpuQueryTotal);
	}

	inline double getCurrentValueTotal() {
		PDH_FMT_COUNTERVALUE counterVal;
		PdhCollectQueryData(cpuQueryTotal);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		return counterVal.doubleValue;
	}

	inline std::vector<double> getCurrentValue() {
		std::vector<double> result;
		PDH_FMT_COUNTERVALUE_ITEM* items = 0;
		DWORD bufsize = 0;
		DWORD itmcount = 0;
		PdhCollectQueryData(cpuQuery);
		auto status = PdhGetFormattedCounterArrayW(cpu, PDH_FMT_DOUBLE, &bufsize, &itmcount, NULL);
		if (status == PDH_MORE_DATA) {
			items = (PDH_FMT_COUNTERVALUE_ITEM*)malloc(bufsize);
			if (items) {
				status = PdhGetFormattedCounterArray(cpu, PDH_FMT_DOUBLE, &bufsize, &itmcount, items);
				if (status == ERROR_SUCCESS) {
					for (int i = 0; i < itmcount; i++) {
						result.push_back(items[i].FmtValue.doubleValue);
					}
				}
				else {
					free(items);
					throw buildException("Error in PdhGetFormattedCouhterArray");
				}
			}
			else throw buildException("Failed to malloc()");
		}
		else throw buildException("Error in PdhGetFormattedCouhterArray");
		free(items);
		return result;
	}

};

class CurrentProcessCpuUsage {

	bool inited = false;
	HANDLE handle = nullptr;
	ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	uint32_t cores = std::thread::hardware_concurrency();

public:

	inline void init() {
		FILETIME ftime, fsys, fuser;
		GetSystemTimeAsFileTime(&ftime);
		memcpy(&lastCPU, &ftime, sizeof(FILETIME));
		handle = GetCurrentProcess();
		GetProcessTimes(handle, &ftime, &ftime, &fsys, &fuser);
		memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
		memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
	}

	inline double getCurrentValue() {
		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;
		long double percent;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes(handle, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		percent = (sys.QuadPart - lastSysCPU.QuadPart) +
			(user.QuadPart - lastUserCPU.QuadPart);
		percent /= (now.QuadPart - lastCPU.QuadPart);
		percent /= cores;
		lastCPU = now;
		lastUserCPU = user;
		lastSysCPU = sys;
		return percent * 100;
	}

};

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
};

struct CpuUsage {
	double total_rate;   // CPU usage rate
	std::vector<double> rate;
};

inline DiskUsage getDiskUsage(char s) {
	size_t avfree = 0; // Available Free Bytes
	size_t total = 0;  // Total Bytes
	size_t free = 0;   // Free Bytes
	const char dname[] = { s, ':', '\0' };
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
	memset(&ms, 0, sizeof(ms));
	ms.dwLength = sizeof(ms);
	auto res = GlobalMemoryStatusEx(&ms);
	if (res) {
		result.physicalUsed = ms.ullTotalPhys - ms.ullAvailPhys;
		result.physicalTotal = ms.ullTotalPhys;
		result.virtualUsed = ms.ullTotalVirtual - ms.ullAvailVirtual;
		result.virtualTotal = ms.ullTotalVirtual;
		//result.rate = ms.dwMemoryLoad;
		return result;
	}
	else throw buildException("Error when trying getting the global memory information");
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
		return result;
	}
	else throw buildException("Error when trying getting the current process memory information");
}

#endif // !SYSTEM_H
