#include "Message.h"
#include "ExtendedJson.h"
#include "Crypt.h"
#include "BDSWebSocket.h"

std::string RawMessage::toJson() {
	nlohmann::json json = *this;
	return json.dump();
}

RawMessage RawMessage::fromJson(const std::string& str) {
	auto json = nlohmann::json::parse(str);
	return json.get<RawMessage>();
}

std::string Message::toJson() {
	nlohmann::json json = *this;
	return json.dump();
}

Message Message::fromJson(const std::string& str) {
	auto json = nlohmann::json::parse(str);
	return json.get<Message>();
}
Message* Message::fromJsonToPtr(const std::string& str) {
	auto msg = new Message;
	auto json = nlohmann::json::parse(str);
	if (json.count("id")) {
		msg->id = json.at("id").get<std::string>();
	}
	if (json.count("type")) {
		msg->type = json.at("type").get<std::string>();
	}
	else if (json.count("event")) {
		msg->event = json.at("event").get<std::string>();
	}
	else {
		throw std::exception("Messages must have key-value 'event' or 'type'");
	}
	msg->data = json.at("data");
	return msg;
}


std::string Message::encryptJson() {
	auto str = toJson();
	return base64_aes_cbc_encrypt(str, bdsws->getAESKey());
}