#ifndef LANGPACK_H
#define LANGPACK_H
#include "pch.h"
#include "nlohmann/json.hpp"

class LangPack {

private:

	std::unordered_map<std::string, std::string> lang;
	nlohmann::json default_lang{
		{"en", {
				{"ws.onopen", "WebSocket client connected!"},
				{"ws.onclose", "WebSocket client disconnected!"},
			}
		},
		{"zh-cn", {}}
	};

	inline void writeDefault(const std::string& fn) {
		std::fstream file(fn, std::ios::out | std::ios::ate);
		file << std::setw(4) << default_lang;
		file.close();
	}

	inline void init(const std::string& fn, const std::string& language) {
		nlohmann::json json;
		auto lastSlash = fn.find_last_of('/');
		auto lastBackslash = fn.find_last_of('\\');
		fs::create_directories(fn.substr(0, max((lastSlash != std::string::npos ? lastSlash : 0),
			(lastBackslash != std::string::npos ? lastBackslash : 0))));
		std::fstream file(fn, std::ios::in | std::ios::app);
		std::ostringstream oss;
		bool flag = false;
		oss << file.rdbuf();
		file.close();
		if (oss.str().empty()) {
			writeDefault(fn);
		}

		try {
			json = json.parse(oss.str());
		}
		catch (nlohmann::detail::exception e) {
			Logger::Error() << "Error when parse JSON!!! " << e.what() <<
				" => Content: " << oss.str() << Logger::endl;
			Logger::Info() << "Try rewriting langpack file...\n";
			writeDefault(fn);
			if (!default_lang.count(language)) {
				Logger::Warn() << "Unknown language name, please check config file! Default English\n";
				lang = default_lang.at("en").get<std::unordered_map<std::string, std::string>>();
			}
			else lang = default_lang.at(language).get<std::unordered_map<std::string, std::string>>();
			return;
		}

		if (!json.count(language)) {
			Logger::Error() << "Unknown language name, please check config file! Default English\n";
			lang = default_lang.at("en").get<std::unordered_map<std::string, std::string>>();
			return;
		}
		lang = json.at(language).get<std::unordered_map<std::string, std::string>>();
	}

public:

	LangPack(const std::string& file, const std::string& language) {
		init(file, language);
	}

	inline std::string get(const std::string& key) {
		if (lang.count(key)) {
			return lang.at(key);
		}
		Logger::Warn() << "Could not find the translation for " << key << Logger::endl;
		return key;
	}
	
};

#endif // !LANGPACK_H
