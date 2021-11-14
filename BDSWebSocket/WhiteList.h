#ifndef WHITELIST_H
#define WHITELIST_H
#include "pch.h"
#include "nlohmann/json.hpp"
#include "BDSWebSocket.h"
#include <loader/Loader.h>

class WhiteList {
	
	inline nlohmann::json read() {
		std::fstream file("whitelist.json", std::ios::in);
		std::ostringstream oss;
		oss << file.rdbuf();
		file.close();
		return nlohmann::json::parse(oss.str());
	}

	inline void write(const nlohmann::json& json) {
		std::fstream file("whitelist.json", std::ios::out | std::ios::ate);
		file << json;
		file.close();
	}

public:

	inline size_t size() {
		return read().size();
	}

	inline std::unordered_map<std::string, xuid_t> list() {
		std::unordered_map<std::string, xuid_t> result;
		for (auto& plInfo : read()) {
			result.emplace(plInfo.at("name").get<std::string>(), 
				atoll(plInfo.at("xuid").get<std::string>().c_str()));
		}
		return result;
	}

	inline WhiteList& add(const std::string& name,
		const std::string& xuid = "", bool ignoresLimit = false) {
		auto wl = read();
		nlohmann::json plInfo{ {"name",name},{"ignoresPlayerLimit",ignoresLimit} };
		if (!xuid.empty()) plInfo["xuid"] = xuid;
		wl.push_back(plInfo);
		write(wl);
		return *this;
	}
	inline WhiteList& add(const std::string& name, xuid_t xuid = 0, bool ignoresLimit = false) {
		auto xuidstr = (xuid ? std::to_string(xuid) : "");
		add(name, xuidstr, ignoresLimit);
		return *this;
	}
	
	inline WhiteList& remove(const std::string& name) {
		auto wl = read();
		int pos = 0;
		for (auto& plInfo : wl) {
			if (plInfo.at("name").get<std::string>() == name) {
				break;
			}
			pos++;
		}
		wl.erase(pos);
		write(wl);
		return *this;
	}

	inline WhiteList& remove(xuid_t xuid) {
		auto wl = read();
		auto xuidstr = std::to_string(xuid);
		int pos = 0;
		for (auto& plInfo : wl) {
			if (plInfo.at("xuid").get<std::string>() == xuidstr) {
				break;
			}
			pos++;
		}
		wl.erase(pos);
		write(wl);
		return *this;
	}

	inline WhiteList& reload() {
		SymCall(Symbol::WhitelistFile::reload, int, void*)(bdsws->wlfile);
		return *this;
	}

};

#endif // !WHITELIST_H
