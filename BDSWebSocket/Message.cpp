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

std::string Message::encryptJson() {
	auto str = toJson();
	return base64_aes_cbc_encrypt(str, bdsws->getAESKey());
}