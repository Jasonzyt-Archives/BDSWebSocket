#ifndef PCH_H
#define PCH_H
// Windows header
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// C++ headers
/// Stream
#include <iostream>
#include <fstream>
#include <sstream>
/// C headers
#include <cstdio>
/// Container
#include <queue>
#include <unordered_map>
#include <vector>
#include <map>
/// Others
#include <mutex>
#include <algorithm>
#include <filesystem>

#define BUILD_RELEASE
#define BDS_1_16
//#define BDS_LATEST

#include "Logger.h"
#include "Symbols.h"
#include "Mess.h"

typedef unsigned long long xuid_t;

namespace fs = std::filesystem;
static const std::string PLUGIN_FOLDER = "./plugins/BDSWebSocket/";
static const std::string PLUGIN_CONFIG = PLUGIN_FOLDER + "config.json";
static const std::string PLUGIN_LANGPK = PLUGIN_FOLDER + "langpack.json";
static const std::string WEBSOCKET_LOG = PLUGIN_FOLDER + "websocket.log";

class BDSWebSocket;
extern std::unique_ptr<BDSWebSocket> bdsws;

#endif // !PCH_H
