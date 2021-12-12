#ifndef OFFLINE_STORAGE_H
#define OFFLINE_STORAGE_H
#include "pch.h"
#include "ExtendedJson.h"
#include <nlohmann/json.hpp>

struct PlayerData {
	std::string name;
	std::string realName;
	xuid_t xuid;
	std::string uuid;
	long long uid;
	time_t lastJoin;
	time_t lastLeft;
};

class OfflineStorage {

	std::vector<PlayerData> plData;
	std::string path;

public:

	OfflineStorage(const std::string& fpath): path(fpath) {}

	void init() {
		std::string content;
		nlohmann::json json;
		if (!fs::exists(path)) {
			std::fstream fstm(path, std::ios::out | std::ios::app);
			fstm << "[]";
			fstm.close();
			content = "[]";
		}
		else {
			std::fstream fstm(path, std::ios::in);
			std::ostringstream oss;
			oss << fstm.rdbuf();
			fstm.close();
			content = oss.str();
		}
		// Parse
		try {
			json = nlohmann::json::parse(content);
		}
		catch (nlohmann::detail::exception e) {
			Logger::Error() << "Error when parse JSON!!! " << e.what() <<
				" => Content: " << content << Logger::endl;
		}
		// Deserialize
		try {
			json.get_to(plData);
		}
		catch (nlohmann::detail::exception e) {
			Logger::Error() << "Error when deserialize JSON!!!" << e.what() << Logger::endl;
			fs::remove(path);
			this->init();
		}
	}

	inline void save() {
		// Serialize
		nlohmann::json json = plData;
		std::fstream fstm(path, std::ios::out | std::ios::trunc);
		fstm << json.dump();
		fstm.close();
	}

	inline PlayerData& getByXuid(xuid_t xuid) {
		for (auto& data : plData) {
			if (data.xuid == xuid) {
				return data;
			}
		}
		throw std::exception("Player not found!");
	}

	inline PlayerData& getByUuid(std::string uuid) {
		for (auto& data : plData) {
			if (data.uuid == uuid) {
				return data;
			}
		}
		throw std::exception("Player not found!");
	}

	inline PlayerData& getByRealName(std::string realName) {
		for (auto& data : plData) {
			if (data.realName == realName) {
				return data;
			}
		}
		throw std::exception("Player not found!");
	}

	inline bool hasPlayer(xuid_t xuid) {
		for (auto& data : plData) {
			if (data.xuid == xuid) {
				return true;
			}
		}
		return false;
	}

	inline PlayerData& newPlayer() {
		PlayerData pdata;
		plData.push_back(pdata);
		return plData.back();
	}

};

#endif // !OFFLINE_STORAGE_H
