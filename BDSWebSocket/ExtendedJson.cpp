#include "ExtendedJson.h"
#include "Message.h"

using namespace std;

void to_json(nlohmann::json& json, const RawMessage& msg) {
	json = nlohmann::json{
		{"data", msg.data},
		{"mode", msg.mode},
		{"encrypted",msg.encrypted}
	};
}

void from_json(const nlohmann::json& json, RawMessage& msg) {
	json.at("data").get_to<std::string>(msg.data);
	json.at("mode").get_to<std::string>(msg.mode);
	json.at("encrypted").get_to<bool>(msg.encrypted);
}

void to_json(nlohmann::json& json, const Message& msg) {
	if (!msg.event.empty()) json["event"] = msg.event;
	else if (!msg.type.empty()) json["type"] = msg.type;
	else return;
	json = nlohmann::json{
		{"id", msg.id},
		{"data", msg.data}
	};
}

void from_json(const nlohmann::json& json, Message& msg) {
	if (json.count("type"))
		msg.type = json.at("type").get<string>();
	else if (json.count("event"))
		msg.event = json.at("event").get<string>();
	else return;
	msg.id = json.at("id").get<string>();
	msg.data = json.at("data").get<Message::Data>();
}

void to_json(nlohmann::json& json, const Message::Data& msg) {
	for (auto& it : msg.stringData) {
		if (it.second != nullptr) {
			json[it.first] = *(it.second);
		}
	}
	for (auto& it : msg.booleanData) {
		if (it.second != nullptr) {
			json[it.first] = *(it.second);
		}
	}
	for (auto& it : msg.ullData) {
		if (it.second != nullptr) {
			json[it.first] = *(it.second);
		}
	}
}

void from_json(const nlohmann::json& json, Message::Data& msg) {
	for (auto& it : msg.stringData) {
		it.second.reset(new string(json.at(it.first).get<string>()));
	}
	for (auto& it : msg.booleanData) {
		it.second.reset(new bool(json.at(it.first).get<bool>()));
	}
	for (auto& it : msg.ullData) {
		it.second.reset(new unsigned long long(json.at(it.first).get<unsigned long long>()));
	}
}