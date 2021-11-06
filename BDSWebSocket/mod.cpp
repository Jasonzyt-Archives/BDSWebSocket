#include <loader/Loader.h>
#include "pch.h"
#include "Config.h"
#include "LangPack.h"
#include "Crypt.h"

using namespace std;
using namespace Logger;

Config* cfg = nullptr;
LangPack* lpk = nullptr;

void entry() {
	cout << "BDS WebSocket loaded! Author: Jasonzyt" << endl;
	cfg = Config::read();
	lpk = new LangPack(PLUGIN_LANGPK, cfg->language);
}

