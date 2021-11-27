#ifndef MC_PROPERTIES_SETTINGS_H
#define MC_PROPERTIES_SETTINGS_H
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////
// struct PropertiesSettings
// level-seed	                                           +0
// level-name                                              +32
// level-type                               string         +64
// server-name                              string         +96
// server-port                              ushort         +208
// server-portv6                            ushort         +210
// max-players                              int            +212
// op-permission-level                      int            +216
// difficulty                               string         +224
// server-type                                             +256
// gamemode                                 string         +288
// language                                 string         +320
// server-id                                string         +352
// max-threads                              int            +384
// tick-distance                            int            +388
// trusted-key                              vector<string> +392
// white-list                               bool           +416
// online-mode                              bool           +417
// force-gamemode                           bool           +418
// allow-cheats                             bool           +419
// texturepack-required                     bool           +420
// msa-gamertags-only                       bool           +421
// content-log-file-enabled                 bool           +422
// enable-item-stack-net-manager-deprecated bool           +423
// view-distance                            bool           +424
// player-idle-timeout                      long           +428
// default-player-permission-level                         +432
// server-wakeup-frequency                  long           +464
// server-authoritative-movement            bool           +468
// player-movement-distance-threshold       float          +472
// player-movement-duration-threshold-in-ms long           +480
// player-movement-score-threshold          float          +488
// correct-player-movement                  bool           +492
// wserver-retry-time                       float          +496
// wsserver-encryption                      bool           +500
// remote-server-communication-endpoint                    ?
// compression-threshold                    long           +502
///////////////////////////////////////////////////////////////////

struct PropertiesSettings {
	std::string level_seed;                        // 0
	std::string level_name;                        // 32
	std::string level_type;                        // 64
	std::string server_name;                       // 96
	std::string unk1;                              // 128
	char unk2[48];                                 // 160
	uint16_t port;                                 // 208
	uint16_t portv6;                               // 210
	uint32_t max_players;                          // 212
	int64_t op_perm_lvl;                           // 216
	std::string difficulty;                        // 224
	std::string server_type;                       // 256
	std::string gamemode;                          // 288
	std::string language;                          // 320
	std::string server_id;                         // 352
	uint32_t max_threads;                          // 384
	uint32_t tick_distance;                        // 388
	std::vector<std::string> trusted_keys;         // 392
	bool whitelist;                                // 416 
	bool online_mode;                              // 417
	bool force_gamemode;                           // 418
	bool allow_cheats;                             // 419
	bool texturepack_required;                     // 420
	bool msa_gamertags_only;                       // 421
	bool content_log_file_enabled;                 // 422
	bool enable_item_stack_net_manager_deprecated; // 423
	uint32_t view_distance;                        // 424
	uint32_t player_idle_timeout;                  // 428
	uint32_t default_player_permission_level;      // 432
	char useless[68];                              // 436
};
#endif // !MC_PROPERTIES_SETTINGS_H
