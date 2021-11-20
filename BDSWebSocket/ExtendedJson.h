#ifndef EXTENDEDJSON_H
#define EXTENDEDJSON_H
#include "nlohmann/json.hpp"
#include "Message.h"

class RawMessage;
class Message;
struct DiskUsage;
struct MemoryUsage;
struct CpuUsage;

void to_json(nlohmann::json& json, const RawMessage& msg);
void from_json(const nlohmann::json& json, RawMessage& msg);

void to_json(nlohmann::json& json, const Message& msg);
void from_json(const nlohmann::json& json, Message& msg);

void to_json(nlohmann::json& json, const DiskUsage& du);
void from_json(const nlohmann::json& json, DiskUsage& du);

void to_json(nlohmann::json& json, const MemoryUsage& du);
void from_json(const nlohmann::json& json, MemoryUsage& du);

void to_json(nlohmann::json& json, const CpuUsage& du);
void from_json(const nlohmann::json& json, CpuUsage& du);

#endif // !EXTENDEDJSON_H
