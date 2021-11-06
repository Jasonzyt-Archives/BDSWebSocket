#ifndef CONFIG_H
#define CONFIG_H
#include "pch.h"
#include "mess.h"
#include "nlohmann/json.hpp"

class Config {
	
public:

	unsigned short port = 0; // 1-65535

	std::vector<std::string> IPWhiteList;

	std::string password;

	std::string language;

	inline void write() {
		nlohmann::json json;
		json["WS_Port"] = port;
		json["WS_IPWhiteList"] = IPWhiteList;
		json["WS_Password"] = password;
		json["Language"] = language;
		std::fstream file(PLUGIN_CONFIG, std::ios::out | std::ios::ate);
		file << std::setw(4) << json;
		file.close();
	}

	static void write(const nlohmann::json& json) {
		std::fstream file(PLUGIN_CONFIG, std::ios::out | std::ios::ate);
		file << std::setw(4) << json;
		file.close();
	}

	static Config* read() {
		auto result = new Config;
		nlohmann::json json;
		fs::create_directories(PLUGIN_FOLDER);
		std::fstream file(PLUGIN_CONFIG, std::ios::in | std::ios::app);
		std::ostringstream oss;
		bool flag = false;
		oss << file.rdbuf();
		file.close();

		try {
			json = json.parse(oss.str());
		}
		catch (nlohmann::detail::exception e) {
			Logger::Error() << "Error when parse JSON!!! " << e.what() <<
				" => Content: " << oss.str() << Logger::endl;
		}
		
		if (!json.count("WS_Port") || !json.at("WS_Port").is_number_unsigned()) {
			json["WS_Port"] = 11451;
			flag = true;
		}
		if (!json.count("WS_IPWhiteList") || !json.at("WS_IPWhiteList").is_array()) {
			json["WS_IPWhiteList"] = std::vector<std::string>{ "127.0.0.1" };
			flag = true;
		}
		if (!json.count("WS_Password") || !json.at("WS_Password").is_string()) {
			json["WS_Password"] = randomString(32);
			flag = true;
		}
		if (!json.count("Language") || !json.at("Language").is_string()) {
			json["Language"] = "en";
			flag = true;
		}
		if (flag) { 
			Logger::Info() << "The config file(" << PLUGIN_CONFIG << 
				") has been automatically modified, please check" << Logger::endl;
			result->write(json);
			return result;
		}
		result->port = json.at("WS_Port").get<unsigned short>();
		result->IPWhiteList = json.at("WS_IPWhiteList").get<std::vector<std::string>>();
		result->password = json.at("WS_Password").get<std::string>();
		result->language = json.at("Language").get<std::string>();
		return result;
	}
};

#endif // !CONFIG_H

