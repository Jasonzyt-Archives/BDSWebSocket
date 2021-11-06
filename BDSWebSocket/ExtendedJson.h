#ifndef EXTENDEDJSON_H
#define EXTENDEDJSON_H
#include "nlohmann/json.hpp"
#include "Message.h"

void to_json(nlohmann::json& json, const RawMessage& msg);
void from_json(const nlohmann::json& json, RawMessage& msg);

void to_json(nlohmann::json& json, const Message& msg);
void from_json(const nlohmann::json& json, Message& msg);

void to_json(nlohmann::json& json, const Message::Data& msg);
void from_json(const nlohmann::json& json, Message::Data& msg);

#endif // !EXTENDEDJSON_H
