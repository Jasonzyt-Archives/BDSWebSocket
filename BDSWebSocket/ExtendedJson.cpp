#include "ExtendedJson.h"
#include "Message.h"
#include "Mess.h"

using namespace std;

void to_json(nlohmann::json& json, const RawMessage& msg) {
	json = nlohmann::json{
		{"data", msg.data},
		{"mode", msg.mode},
		{"encrypted",msg.encrypted}
	};
}
void from_json(const nlohmann::json& json, RawMessage& msg) {
	json.at("data").get_to(msg.data);
	json.at("mode").get_to(msg.mode);
	json.at("encrypted").get_to<bool>(msg.encrypted);
}

void to_json(nlohmann::json& json, const Message& msg) {
	json = nlohmann::json{
		{"data", msg.data}
	};
	if (!msg.id.empty()) {
		json["id"] = msg.id;
	}
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
	if (json.count("data")) {
		Logger::Info("wwww???\n");
		msg.data = json.at("data");
	}
	if (json.count("id")) {
		json.at("id").get_to(msg.id);
	}

	if (json.count("type")) {
		json.at("type").get_to(msg.type);
	}
	else if (json.count("event")) {
		json.at("event").get_to(msg.event);
	}
	else {
		return;
	}
}

void to_json(nlohmann::json& json, const DiskUsage& du) {
	json = nlohmann::json{
		{"total", du.total},
		{"avfree", du.avfree},
		{"free", du.free},
		{"type", du.type}
	};
}
void from_json(const nlohmann::json& json, DiskUsage& du) {
	json.at("total").get_to(du.total);
	json.at("avfree").get_to(du.avfree);
	json.at("free").get_to(du.free);
	json.at("type").get_to(du.type);
}

void to_json(nlohmann::json& json, const MemoryUsage& mu) {
	json = nlohmann::json{
		{"physicalUsed", mu.physicalUsed},
		{"physicalTotal", mu.physicalTotal},
		{"virtualUse", mu.virtualUsed},
		{"virtualTotal", mu.virtualTotal},
		{"rate", mu.rate}
	};
}
void from_json(const nlohmann::json& json, MemoryUsage& du) {
	json.at("physicalUsed").get_to(du.physicalUsed);
	json.at("physicalTotal").get_to(du.physicalTotal);
	json.at("virtualUse").get_to(du.virtualUsed);
	json.at("virtualTotal").get_to(du.virtualTotal);
	json.at("rate").get_to(du.rate);
}

void to_json(nlohmann::json& json, const CpuUsage& cu) {
	json = nlohmann::json{
		{"rate", cu.rate}
	};
	if (cu.core > 0) {
		json["core"] = cu.core;
	}
}
void from_json(const nlohmann::json& json, CpuUsage& cu) {
	json.at("rate").get_to(cu.rate);
	if (json.count("core")) {
		json.at("core").get_to(cu.core);
	}
}