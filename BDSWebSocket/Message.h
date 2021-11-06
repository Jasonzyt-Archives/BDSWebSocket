#ifndef MESSAGE_H
#define MESSAGE_H
#include "pch.h"

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

	class Data {

	public:

		std::unordered_map<std::string, std::unique_ptr<std::string>&> stringData{
			{"name", name}, {"nick", nick}, {"reason", reason}, {"cmd", cmd}
		};
		std::unordered_map< std::string, std::unique_ptr<bool>&> booleanData{
			{"success", success}, {"isFakePlayer", isFakePlayer}
		};
		std::unordered_map<std::string, std::unique_ptr<unsigned long long>&> ullData{
			{"xuid", xuid}
		};

		std::unique_ptr<std::string> name;
		std::unique_ptr<std::string> nick;
		std::unique_ptr<std::string> reason;
		std::unique_ptr<std::string> cmd;
		std::unique_ptr<xuid_t> xuid;
		std::unique_ptr<bool> success;
		std::unique_ptr<bool> isFakePlayer;
	};

	std::string id;
	std::string event;
	std::string type;
	Data data;

	std::string toJson();

	static Message fromJson(const std::string& str);

};

#endif // !MESSAGE_H
