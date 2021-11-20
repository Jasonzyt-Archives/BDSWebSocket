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
	if (json.count("data")) {
		msg->data = json.at("data");
	}

	if (json.count("id")) {
		msg->id = json.at("id");
	}
	if (json.count("type")) {
		msg->type = json.at("type");
	}
	else if (json.count("event")) {
		msg->event = json.at("event");
	}
	else {
		throw std::exception("Messages must have key-value 'event' or 'type'");
	}
	return msg;
}


std::string Message::encryptJson() {
	auto str = toJson();
	return base64_aes_cbc_encrypt(str, bdsws->getAESKey());
}