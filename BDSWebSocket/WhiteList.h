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

	template<typename KT, typename VT>
	inline bool hasValue(const std::unordered_map<KT, VT>& mp, VT val) {
		for (auto& [k, v] : mp) {
			if (v == val) return true;
		}
		return false;
	}

public:

	inline size_t size() {
		return read().size();
	}
	
	inline std::unordered_map<std::string, xuid_t> list() {
		std::unordered_map<std::string, xuid_t> result;
		for (auto& plInfo : read()) {
			xuid_t xid = 0;
			if (plInfo.count("xuid")) {
				xid = atoll(plInfo.at("xuid").get<std::string>().c_str());
			}
			result.emplace(plInfo.at("name"), xid);
		}
		return result;
	}
	inline std::unordered_map<std::string, std::string> list2() {
		std::unordered_map<std::string, std::string> result;
		for (auto& plInfo : read()) {
			std::string xid;
			if (plInfo.count("xuid")) {
				xid = plInfo.at("xuid");
			}
			result.emplace(plInfo.at("name"), xid);
		}
		return result;
	}

	inline WhiteList& add(const std::string& name,
		const std::string& xuid = "", bool ignoresLimit = false) {
		auto wl = read();
		auto list = this->list2();
		if (list.count(name) && !list.at(name).empty())
			throw std::exception((name + " already exists in the whitelist").c_str());
		if (hasValue(list,xuid)) 
			throw std::exception(("Xuid" + xuid + " already exists in the whitelist").c_str());
		nlohmann::json plInfo{ {"name",name},{"ignoresPlayerLimit",ignoresLimit} };
		if (!xuid.empty()) plInfo["xuid"] = xuid;
		wl.push_back(plInfo);
		write(wl);
		Logger::Info() << "Whitelist added {" << name << ", " << xuid << ", " << ignoresLimit << "}\n";
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
		bool found = false;
		for (auto& plInfo : wl) {
			if (plInfo.at("name").get<std::string>() == name) {
				found = true;
				break;
			}
			pos++;
		}
		if (!found) 
			throw std::exception(("Couldn't find " + name + " in whitelist!").c_str());
		wl.erase(pos);
		write(wl);
		Logger::Info() << "Whitelist removed {" << name << "}\n";
		return *this;
	}
	inline WhiteList& remove(xuid_t xuid) {
		auto wl = read();
		auto xuidstr = std::to_string(xuid);
		int pos = 0;
		bool found = false;
		for (auto& plInfo : wl) {
			if (plInfo.at("xuid").get<std::string>() == xuidstr) {
				found = true;
				break;
			}
			pos++;
		}
		if (!found)
			throw std::exception(("Couldn't find " + xuidstr + " in whitelist!").c_str());
		wl.erase(pos);
		write(wl);
		Logger::Info() << "Whitelist removed {" << xuid << "}\n";
		return *this;
	}

	inline WhiteList& reload() {
		SymCall(Symbol::WhitelistFile::reload, int, void*)(bdsws->wlfile);
		return *this;
	}

};

#endif // !WHITELIST_H
