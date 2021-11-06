#include "Message.h"
#include "ExtendedJson.h"

std::string RawMessage::toJson() {
	nlohmann::json json = *this;
	return json.dump();
}

RawMessage RawMessage::fromJson(const std::string& str) {
	nlohmann::json json;
	json.parse(str);
	return json.get<RawMessage>();
}

std::string Message::toJson() {
	nlohmann::json json = *this;
	return json.dump();
}

Message Message::fromJson(const std::string& str) {
	nlohmann::json json;
	json.parse(str);
	return json.get<Message>();
}