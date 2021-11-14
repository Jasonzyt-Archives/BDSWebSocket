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
		std::string mode = "AES/CBC/PKCS5Padding"
	) {
		this->data = data;
		this->mode = mode;
		this->encrypted = encrypted;
	}

	std::string toJson();

	static RawMessage fromJson(const std::string& str);

};

class Message {
	
public:

	Message() = default;

	std::string id;
	std::string event;
	std::string type;
	nlohmann::json data;

	std::string toJson();

	static Message fromJson(const std::string& str);

	std::string encryptJson();

};

#endif // !MESSAGE_H
