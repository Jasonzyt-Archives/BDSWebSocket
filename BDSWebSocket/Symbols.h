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
		// Real function name: ServerNetworkHandler::handle
		// onTextPacket(Chat)
		// void | ServerNetworkHandler* thiz, NetworkIdentifier*, TextPacket*
		SYMBOL handle_TextPacket =
			"?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z";
		// getServerPlayer
		// ServerPlayer* | ServerNetworkHandler* thiz, NetworkIdentifier*, unsigned char(ClientSubId)
		SYMBOL _getServerPlayer = "?_getServerPlayer@ServerNetworkHandler@@AEAAPEAV"
			"ServerPlayer@@AEBVNetworkIdentifier@@E@Z";
	}
	namespace NetworkHandler {
		// getNetworkPeerForUser
		// NetworkPeer* | NetworkHandler* thiz, NetworkIdentifier*
		SYMBOL getPeerForUser = "?getPeerForUser@NetworkHandler@@QEAAPEAVNetwork"
			"Peer@@AEBVNetworkIdentifier@@@Z";
	}
	namespace MinecraftCommands {
		// onCommand
		// bool | MinecraftCommands* thiz, unsigned int*, std::shared_ptr<CommandContext>, char
		SYMBOL executeCommand = "?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@"
			"@V?$shared_ptr@VCommandContext@@@std@@_N@Z";
	}
	namespace ConnectionRequest {
		// getClientPlatformId
		// __int64(???) | ConnectionRequest* thiz
		SYMBOL getClientPlatformId = "?getClientPlatformId@ConnectionRequest@@QEBA?AV"
			"?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ";
		// getClientPlatformOnlineId
		// std::string | ConnectionRequest* thiz
		SYMBOL getClientPlatformOnlineId = "?getClientPlatformOnlineId@ConnectionRequest"
			"@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ";
		// getDeviceId
		// std::string | ConnectionRequest* thiz
		SYMBOL getDeviceId = "?getDeviceId@ConnectionRequest@@QEBA?AV?$basic_string@DU?"
			"$char_traits@D@std@@V?$allocator@D@2@@std@@XZ";
		// getSkinData
		// std::vector<unsigned char> | ConnectionRequest* thiz
		SYMBOL getSkinData = "?getSkinData@ConnectionRequest@@QEBA?AV?$vector@EV?$"
			"allocator@E@std@@@std@@XZ";
		// getCapeData
		// std::vector<unsigned char> | ConnectionRequest* thiz
		SYMBOL getCapeData = "?getCapeData@ConnectionRequest@@QEBA?AV?$vector@EV?$"
			"allocator@E@std@@@std@@XZ";
		// getData
		// Json::Value | ConnectionRequest* thiz, std::string
		SYMBOL getData = "?getData@ConnectionRequest@@AEBA?AVValue@Json@@AEBV?$basic_string"
			"@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
	}
	namespace WebToken {
		// toString
		// std::string | WebToken* thiz
		SYMBOL toString = "?toString@WebToken@@QEBA?AV?$basic_string@DU?$char_traits@D@std"
			"@@V?$allocator@D@2@@std@@XZ";
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
		// fetchActor
		// Actor* | Level* thiz, ActorUniqueID, bool
		SYMBOL fetchActor = "?fetchEntity@Level@@QEBAPEAVActor@@UActorUniqueID@@_N@Z";
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
	namespace MinecraftPackets {
		// Static function
		// createPacket
		// void* | Packet*, MinecraftPacketIds(int)
		SYMBOL creatPacket = "?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@V"
			"Packet@@@std@@W4MinecraftPacketIds@@@Z";
	}
	namespace NetworkIdentifier {
		// getClientAddress
		// std::string | NetworkIdentifier* thiz
		SYMBOL getAddress = "?getAddress@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$char_"
			"traits@D@std@@V?$allocator@D@2@@std@@XZ";
	}
	namespace ExtendedCertificate {
		// Static function
		// getXuid
		// std::string | Certificate*
		SYMBOL getXuid = "?getXuid@ExtendedCertificate@@SA?AV?$basic_string@DU?$char_traits"
			"@D@std@@V?$allocator@D@2@@std@@AEBVCertificate@@@Z";
		// Static function
		// getIdentityName(RealName or XboxName)
		// std::string | Certificate*
		SYMBOL getIdentityName = "?getIdentityName@ExtendedCertificate@@SA?AV?$basic_string@"
			"DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVCertificate@@@Z";
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
	namespace PropertiesSettings {
		// onPropertiesSettingsConstruct
		// void | PropertiesSettings* thiz, const std::string&
		SYMBOL PropertiesSettings = "??0PropertiesSettings@@QEAA@AEBV?$basic_string@DU?"
			"$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
	}
	namespace WhitelistFile {
		// onWhitelistReload
		// FileReadResult(int) | WhitelistFile* thiz
		SYMBOL reload = "?reload@WhitelistFile@@QEAA?AW4FileReadResult@@XZ";
	}
	namespace Json {
		namespace Value {
			// toString
			// std::string | Json::Value* thiz, std::string
			SYMBOL asString = "?asString@Value@Json@@QEBA?AV?$basic_string@DU?$char_traits"
				"@D@std@@V?$allocator@D@2@@std@@AEBV34@@Z";
			// toInt
			// int | Json::Value* thiz, int
			SYMBOL asInt = "?asInt@Value@Json@@QEBAHH@Z";
		}
	}
	namespace mce {
		namespace UUID {
			// asString
			// std::string | UUID* thiz, std::string*(ret)
			SYMBOL asString = "?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@"
				"D@std@@V?$allocator@D@2@@std@@XZ";
		}
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
	namespace RakNetInstance {
		// getIPv4Port
		// ushort | RakNetInstance* thiz
		SYMBOL getIPv4Port = "?getIPv4Port@RakNetInstance@@UEBAGXZ";
		// getIPv6Port
		// ushort | RakNetInstance* thiz
		SYMBOL getIPv6Port = "?getIPv6Port@RakNetInstance@@UEBAGXZ";
	}
	namespace PropertiesSettings {
		// onPropertiesSettingsConstruct
		// void | PropertiesSettings* thiz, const std::string&
		SYMBOL PropertiesSettings = "??0PropertiesSettings@@QEAA@AEBV?$basic_string@DU?"
			"$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
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

