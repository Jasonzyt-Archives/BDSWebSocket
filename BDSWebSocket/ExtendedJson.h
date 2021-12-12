#ifndef EXTENDEDJSON_H
#define EXTENDEDJSON_H
#include "nlohmann/json.hpp"
#include "Message.h"

#define ToJsonCallBack(type) void to_json(nlohmann::json& json, const type&)
#define FromJsonCallBack(type) void from_json(const nlohmann::json& json, type&)

class Player;
class RawMessage;
class Message;
class Vec3;
class BlockPos;
struct PlayerData;
struct DiskUsage;
struct MemoryUsage;
struct CpuUsage;
struct ActorUniqueID;

ToJsonCallBack(RawMessage);
FromJsonCallBack(RawMessage);

ToJsonCallBack(Message);
FromJsonCallBack(Message);

ToJsonCallBack(DiskUsage);
FromJsonCallBack(DiskUsage);

ToJsonCallBack(MemoryUsage);
FromJsonCallBack(MemoryUsage);

ToJsonCallBack(CpuUsage);
FromJsonCallBack(CpuUsage);

ToJsonCallBack(ActorUniqueID);
FromJsonCallBack(ActorUniqueID);

ToJsonCallBack(Vec3);
FromJsonCallBack(Vec3);

ToJsonCallBack(BlockPos);
FromJsonCallBack(BlockPos);

ToJsonCallBack(Player*); // NOT USED
FromJsonCallBack(Player*);

ToJsonCallBack(PlayerData);
FromJsonCallBack(PlayerData);

#endif // !EXTENDEDJSON_H
