#ifndef SYMBOLS_H
#define SYMBOLS_H

#define SYMBOL static const constexpr char*

namespace Symbol {

#if defined(BDS_1_16)

	namespace ServerNetworkHandler {
		// onPlayerJoin
		// void | ServerNetworkHandler* thiz, NetworkIdentifier*, ConnectionRequest*, ServerPlayer*
		SYMBOL sendLoginMessageLocal =
			"?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@"
			"AEBVConnectionRequest@@AEAVServerPlayer@@@Z";
		// onPlayerLeft
		// void | ServerNetworkHandler* thiz, ServerPlayer*, bool
		SYMBOL _onPlayerLeft =
			"?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z";
		// onTextPacket(Chat)
		// void | ServerNetworkHandler* thiz, NetworkIdentifier*, TextPacket*
		SYMBOL handle_TextPacket =
			"?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z";
		// getServerPlayer
		// ServerPlayer* | ServerNetworkHandler* thiz, NetworkIdentifier*, unsigned char(ClientSubId)
		SYMBOL _getServerPlayer = "?_getServerPlayer@ServerNetworkHandler"
			"@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z";
	}
	namespace MinecraftCommands {
		// onCommand
		// bool | MinecraftCommands* thiz, unsigned int*, std::shared_ptr<CommandContext>, char
		SYMBOL executeCommand =
			"?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z";
	}
	namespace Player {
		// onPlayerDeath
		// void* | ServerPlayer* thiz, const ActorDamageSource&
		SYMBOL die = "?die@Player@@UEAAXAEBVActorDamageSource@@@Z";
	}
	namespace Level {
		// forEachPlayer
		// void | Level* thiz, std::function<bool(Player&)>
		SYMBOL forEachPlayer = "?forEachPlayer@Level@@QEBAXV?$function@$$A6A_NAEBVPlayer@@@Z@std@@@Z";
	}
	namespace ServerLevel {
		// onServerLevelTick
		// void | ServerLevel* thiz
		SYMBOL tick = "?tick@ServerLevel@@UEAAXXZ";
	}
	namespace Minecraft {
		// onServerInit (to get Minecraft*)
		// void | Minecraft* thiz
		SYMBOL initAsDedicatedServer = "?initAsDedicatedServer@Minecraft@@QEAAXXZ";
	}
	namespace ExtendedCertificate {
		// getXuid
		// std::string | ExtendedCertificate* thiz
		SYMBOL getXuid = "?getXuid@ExtendedCertificate@@SA?AV?$basic_string@DU?$char_traits"
			"@D@std@@V?$allocator@D@2@@std@@AEBVCertificate@@@Z";
	}
	namespace ServerInstance {
		// onServerThreadsStart
		// void | ServerInstance* thiz
		SYMBOL startServerThread = "?startServerThread@ServerInstance@@QEAAXXZ";
	}
	namespace RakNetInstance {
		// getIPv4Port
		// ushort | RakNetInstance* thiz
		SYMBOL getIPv4Port = "?getIPv4Port@RakNetInstance@@UEBAGXZ";
		// getIPv6Port
		// ushort | RakNetInstance* thiz
		SYMBOL getIPv6Port = "?getIPv6Port@RakNetInstance@@UEBAGXZ";
	}
	namespace WhitelistFile {
		// onWhitelistReload
		// FileReadResult(int) | WhitelistFile* thiz
		SYMBOL reload = "?reload@WhitelistFile@@QEAA?AW4FileReadResult@@XZ";
	}
#elif defined(BDS_LATEST)

	namespace ServerNetworkHandler {
		// onPlayerJoin
		// void | ServerNetworkHandler* thiz, NetworkIdentifier*, ConnectionRequest*, ServerPlayer*
		SYMBOL sendLoginMessageLocal =
			"?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@"
			"AEBVConnectionRequest@@AEAVServerPlayer@@@Z";
		// onPlayerLeft
		// void | ServerNetworkHandler* thiz, ServerPlayer*, bool
		SYMBOL _onPlayerLeft =
			"?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z";
		// onTextPacket(Chat)
		// void | void* thiz, NetworkIdentifier*, TextPacket*
		SYMBOL handle_TextPacket =
			"?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z";
		// getServerPlayer
		// ServerPlayer* | ServerNetworkHandler* thiz, NetworkIdentifier*, unsigned char
		SYMBOL _getServerPlayer = "?_getServerPlayer@ServerNetworkHandler"
			"@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z";
	}
	namespace MinecraftCommands {
		// onCommand
		// bool | MinecraftCommands* thiz, unsigned int*, std::shared_ptr<CommandContext>, char
		SYMBOL executeCommand =
			"?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z";
	}
	namespace Player {
		// onPlayerDeath
		// void* | ServerPlayer* thiz, const ActorDamageSource&
		SYMBOL die = "?die@Player@@UEAAXAEBVActorDamageSource@@@Z";
	}
	namespace Actor {
		// 1.17 only
		// getComponent
		// UserEntityIdentifierComponent | Actor* thiz
		SYMBOL tryGetComponent_UEIC =
			"??$tryGetComponent@VUserEntityIdentifierComponent@@@Actor@@QEAAPEAV"
			"UserEntityIdentifierComponent@@XZ";
	}
	namespace Level {
		// forEachPlayer
		// void | Level* thiz, std::function<bool(Player&)>
		SYMBOL forEachPlayer = "?forEachPlayer@Level@@UEBAXV?$function@$$A6A_NAEBVPlayer@@@Z@std@@@Z";
	}
	namespace ServerLevel {
		// onServerLevelTick
		// void | ServerLevel* thiz
		SYMBOL tick = "?tick@ServerLevel@@UEAAXXZ";
	}
	namespace Minecraft {
		// onServerInit (to get Minecraft*)
		// void | Minecraft* thiz
		SYMBOL initAsDedicatedServer = "?initAsDedicatedServer@Minecraft@@QEAAXXZ";
	}
	namespace ExtendedCertificate {
		// getXuid
		// std::string | ExtendedCertificate* thiz
		SYMBOL getXuid = "?getXuid@ExtendedCertificate@@SA?AV?$basic_string@DU?$char_traits"
			"@D@std@@V?$allocator@D@2@@std@@AEBVCertificate@@@Z";
	}
	namespace ServerInstance {
		// onServerThreadsStart
		// void | ServerInstance* thiz
		SYMBOL startServerThread = "?startServerThread@ServerInstance@@QEAAXXZ";
	}
	namespace WhitelistFile {
		// onWhitelistReload
		// FileReadResult(int) | WhitelistFile* thiz
		SYMBOL reload = "?reload@WhitelistFile@@QEAA?AW4FileReadResult@@XZ";
	}
#else
	#error "Unknown BDS version"
#endif

}

#endif // !SYMBOLS_H

