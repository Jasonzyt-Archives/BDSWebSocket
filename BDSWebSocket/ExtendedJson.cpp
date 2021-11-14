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
	json = nlohmann::json{
		{"id", msg.id},
		{"data", msg.data}
	};
	if (!msg.event.empty()) {
		json["event"] = msg.event;
	}
	else if (!msg.type.empty()) {
		json["type"] = msg.type;
	}
	else {
		json.clear();
	}
}

void from_json(const nlohmann::json& json, Message& msg) {
	if (json.count("id")) {
		msg.id = json.at("id").get<string>();
	}
	if (json.count("type")) {
		msg.type = json.at("type").get<string>();
	}
	else if (json.count("event")) {
		msg.event = json.at("event").get<string>();
	}
	else {
		return;
	}
	msg.data = json.at("data");
}
