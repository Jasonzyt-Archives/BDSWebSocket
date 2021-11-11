#ifndef SYMBOLS_H
#define SYMBOLS_H

namespace Symbol {

#if defined(BDS_1_16)

	// onPlayerJoin
	// void | ServerNetworkHandler* thiz, NetworkIdentifier*, ConnectionRequest*, ServerPlayer*
	static const constexpr char* ServerNetworkHandler_sendLoginMessageLocal =
		"?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@"
		"AEBVConnectionRequest@@AEAVServerPlayer@@@Z";
	// onPlayerLeft
	// void | ServerNetworkHandler* thiz, ServerPlayer*, bool
	static const constexpr char* ServerNetworkHandler__onPlayerLeft =
		"?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z";
	// onTextPacket(Chat)
	// void | void* thiz, NetworkIdentifier*, TextPacket*
	static const constexpr char* ServerNetworkHandler_handle_TextPacket =
		"?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z";
	// onCommand
	// bool | MinecraftCommands* thiz, unsigned int*, std::shared_ptr<CommandContext>, char
	static const constexpr char* MinecraftCommands_executeCommand =
		"?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z";
	// onPlayerDeath
	// void* | ServerPlayer* thiz, const ActorDamageSource&
	static const constexpr char* Player_die = "?die@Player@@UEAAXAEBVActorDamageSource@@@Z";

#elif defined(BDS_LATEST)

	// onPlayerJoin
	// void | ServerNetworkHandler* thiz, NetworkIdentifier*, ConnectionRequest*, ServerPlayer*
	static const constexpr char* ServerNetworkHandler_sendLoginMessageLocal = 
		"?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@"
		"AEBVConnectionRequest@@AEAVServerPlayer@@@Z";
	// onPlayerLeft
	// void | ServerNetworkHandler* thiz, ServerPlayer*, bool
	static const constexpr char* ServerNetworkHandler__onPlayerLeft = 
		"?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z";
	// onTextPacket(Chat)
	// void | void* thiz, NetworkIdentifier*, TextPacket*
	static const constexpr char* ServerNetworkHandler_handle_TextPacket = 
		"?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z";
	// onCommand
	// bool | MinecraftCommands* thiz, unsigned int*, std::shared_ptr<CommandContext>, char
	static const constexpr char* MinecraftCommands_executeCommand = 
		"?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z";
	// onPlayerDeath
	// void* | ServerPlayer* thiz, const ActorDamageSource&
	static const constexpr char* Player_die = "?die@Player@@UEAAXAEBVActorDamageSource@@@Z";

#else
	#error "Unknown BDS version"
#endif

}

#endif // !SYMBOLS_H

