#ifndef MESSAGE_H
#define MESSAGE_H
#include "pch.h"
#include "nlohmann/json.hpp"

class RawMessage {
	
public:

	std::string data;
	std::string mode;
	bool encrypted;
	
	RawMessage() {}
	RawMessage(
		std::string data, 
		bool encrypted = true,
		std::string mode = "AES/CBC/PKCS7Padding"
	) {
		this->data = data;
		this->encrypted = encrypted;
		if (encrypted) this->mode = mode;
	}

	std::string toJson();

	static RawMessage fromJson(const std::string& str);

};

class Message {
	
public:

	Message() = default;
	Message(const Message& msg) {
		*this = msg;
	}

	std::string id;
	std::string event;
	std::string type;
	nlohmann::json data;

	std::string toJson();

	static Message fromJson(const std::string& str);
	static Message* fromJsonToPtr(const std::string& str);

	std::string encryptJson();

	void operator=(const Message& msg) {
		if (&msg != this) {
			id = msg.id;
			event = msg.event;
			type = msg.type;
			data = msg.data;
		}
	}

};

#endif // !MESSAGE_H
