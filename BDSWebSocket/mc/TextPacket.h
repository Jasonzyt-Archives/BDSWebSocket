#ifndef MC_TEXT_PACKET_H
#define MC_TEXT_PACKET_H
#include <string>
#include <vector>

#if 0
enum TextType;
struct TextPacket {
#if defined(BDS_1_16)
	char basicPacket[32];
#elif defined(BDS_LATEST)
	char basicPacket[40];
#else
#error "BDS version is wrong"
#endif
	TextType type;                         // 32 40
	char align_0[7];                       // 33 41
	std::string* sourceName;               // 40 48
	char unk_1[24];                        // 72 80
	std::string* msg;                      // 80 88
	char unk_2[24];                        // 112
	std::vector<std::string>* params;      // 120
	char unk_3[16];                        // 136
	bool needsTrans;                       // 137
	char align_1[7];                       // 144
	std::string* xuid;                     // 152
	char unk_4[24];                        // 176
	void* platformChatId;                  // 184
};
#endif

#endif // !MC_TEXT_PACKET_H
