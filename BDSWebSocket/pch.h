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
/// Others
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <map>
#include <filesystem>

#include "Logger.h"

typedef unsigned long long xuid_t;

namespace fs = std::filesystem;
static const std::string PLUGIN_FOLDER = "./plugins/BDSWebSocket/";
static const std::string PLUGIN_CONFIG = PLUGIN_FOLDER + "config.json";
static const std::string PLUGIN_LANGPK = PLUGIN_FOLDER + "langpack.json";
static const std::string WEBSOCKET_LOG = PLUGIN_FOLDER + "websocket.log";

#endif // !PCH_H
