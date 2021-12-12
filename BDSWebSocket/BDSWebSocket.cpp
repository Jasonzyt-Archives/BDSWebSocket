#define CROW_MAIN
#include "BDSWebSocket.h"
#include "LangPack.h"
#include "Crypt.h"
#include "OfflineStorage.h"

AESKey BDSWebSocket::getAESKey() {
	AESKey ak;
	auto md5passwd = md5(cfg->password);
	auto key = md5passwd.substr(0, 16);
	auto iv = md5passwd.substr(16);
	for (int i = 0; i < 16; i++) {
		ak.key[i] = (unsigned char)key[i];
	}
	for (int i = 0; i < 16; i++) {
		ak.iv[i] = (unsigned char)iv[i];
	}
	ak.key[16] = '\0';
	ak.iv[16] = '\0';
	return ak;
}

BDSWebSocket::BDSWebSocket(Config* cfg) {
	this->cfg.reset(cfg);
	this->lpk.reset(new LangPack(PLUGIN_LANGPK, cfg->language));
	this->ws.reset(new WebSocketServer);
	this->ols.reset(new OfflineStorage(OFFLINE_DATA));
	ols->init();
	ols->checkData();
	gcu.init();
	cpcu.init();
}

BDSWebSocket::~BDSWebSocket() {
}