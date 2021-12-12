#include "ExtendedJson.h"
#include "Message.h"
#include "System.h"
#include "OfflineStorage.h"
#include "Mess.h"
#include <mc/Player.h>

using namespace std;

// RawMessage
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

// Message
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

// DiskUsage
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

// MemoryUsage
void to_json(nlohmann::json& json, const MemoryUsage& mu) {
	json = nlohmann::json{
		{"physicalUsed", mu.physicalUsed},
		{"physicalTotal", mu.physicalTotal},
		{"virtualUsed", mu.virtualUsed},
		{"virtualTotal", mu.virtualTotal},
	};
}
void from_json(const nlohmann::json& json, MemoryUsage& du) {
	json.at("physicalUsed").get_to(du.physicalUsed);
	json.at("physicalTotal").get_to(du.physicalTotal);
	json.at("virtualUsed").get_to(du.virtualUsed);
	json.at("virtualTotal").get_to(du.virtualTotal);
}

// CpuUsage
void to_json(nlohmann::json& json, const CpuUsage& cu) {
	json = nlohmann::json{
		{"totalRate", cu.total_rate}
	};
	if (!cu.rate.empty()) json["rate"] = cu.rate;
}
void from_json(const nlohmann::json& json, CpuUsage& cu) {
	json.at("totalRate").get_to(cu.total_rate);
	json.at("rate").get_to(cu.rate);
}

// ActorUniqueID
void to_json(nlohmann::json& json, const ActorUniqueID& uid) {
	auto pl = getPlayerByUniqueID(uid);
	if (uid.id != -1 && pl) {
		auto pl = getPlayerByUniqueID(uid);
		json = nlohmann::json{
			{"name", pl->getNameTag()},
			{"realName", getRealName(const_cast<Player*>(pl))},
			{"xuid", getXuid(const_cast<Player*>(pl))},
			{"uuid", getUuid(const_cast<Player*>(pl)).asString()},
			{"uniqueID", uid.id},
			{"position", pl->getPos()}
		};
		return;
	}
	throw buildException("None of the players here have a UniqueID of ", uid.id);
}
void from_json(const nlohmann::json& json, ActorUniqueID& uid) {
	uid = ActorUniqueID(json.get<int64_t>());
}

// Vec3
void to_json(nlohmann::json& json, const Vec3& pos) {
	json = {
		{"x", pos.x},
		{"y", pos.y},
		{"z", pos.z}
	};
}
void from_json(const nlohmann::json& json, Vec3& pos) {
	json.at("x").get_to(pos.x);
	json.at("y").get_to(pos.y);
	json.at("z").get_to(pos.z);
}

// BlockPos
void to_json(nlohmann::json& json, const BlockPos& pos) {
	json = {
		{"x", pos.x},
		{"y", pos.y},
		{"z", pos.z}
	};
}
void from_json(const nlohmann::json& json, BlockPos& pos) {
	json.at("x").get_to(pos.x);
	json.at("y").get_to(pos.y);
	json.at("z").get_to(pos.z);
}

// Player*
void to_json(nlohmann::json& json, const Player*& pl) {
	// NOT USED
}
void from_json(const nlohmann::json& json, Player*& pl) {
	pl = nullptr;
	if (json.is_object()) {
		if (json.count("xuid")) {
			if (json.at("xuid").is_string()) {
				pl = getPlayerByXuid(stoull(json.at("xuid").get<std::string>()));
			}
			else {
				pl = getPlayerByXuid(json.at("xuid").get<xuid_t>());
			}
		}
		else if (json.count("uuid")) {
			pl = getPlayerByUuid(json.at("uuid").get<string>());
		}
		else if (json.count("uniqueID")) {
			pl = getPlayerByUniqueID(json.at("uniqueID").get<int64_t>());
		}
		else if (json.count("realName")) {
			pl = getPlayerByRealName(json.at("realName").get<string>());
		}
		else if (json.count("name")) {
			pl = getPlayerByName(json.at("name").get<string>());
		}
	}
}

void to_json(nlohmann::json& json, const PlayerData& pdata) {
	json["name"] = pdata.name;
	json["realName"] = pdata.realName;
	json["xuid"] = pdata.xuid;
	json["uuid"] = pdata.uuid;
	json["uid"] = pdata.uid;
	json["lastJoin"] = pdata.lastJoin;
	json["lastLeft"] = pdata.lastLeft;
}
void from_json(const nlohmann::json& json, PlayerData& pdata) {
	json.at("name").get_to(pdata.name);
	json.at("realName").get_to(pdata.realName);
	json.at("xuid").get_to(pdata.xuid);
	json.at("uuid").get_to(pdata.uuid);
	json.at("uid").get_to(pdata.uid);
	json.at("lastJoin").get_to(pdata.lastJoin);
	json.at("lastLeft").get_to(pdata.lastLeft);
}
