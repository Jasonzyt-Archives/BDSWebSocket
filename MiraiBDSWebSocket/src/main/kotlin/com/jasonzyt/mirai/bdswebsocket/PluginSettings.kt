package com.jasonzyt.mirai.bdswebsocket

import net.mamoe.mirai.console.data.AutoSavePluginConfig
import net.mamoe.mirai.console.data.ValueDescription
import net.mamoe.mirai.console.data.value
import kotlinx.serialization.*

@Serializable
data class ServerConfig(
    val serverID: String = "服务器唯一标识",
    val serverName: String = "",
    val default: Boolean = false,
    val wsUri: String = "ws://127.0.0.1:11451",
    val selfAddWhitelist: Boolean = false,
    val forwardChat: Boolean = true,
    val forwardCmd: Boolean = true,
    val forwardJoin: Boolean = true,
    val forwardLeft: Boolean = true,
    val password: String = "AES Key",
    val targetGroups: MutableList<Long> = mutableListOf(1145141919810),
)

object PluginSettings : AutoSavePluginConfig("settings") {
    @ValueDescription("BDS插件WebSocket服务器列表")
    var servers by value(mutableListOf(ServerConfig()))
    @ValueDescription("是否允许成员改绑XboxID")
    val allowRebind by value(false)
    @ValueDescription("转发聊天信息格式")
    val chatFormat by value("{name} >> {chat}")
    @ValueDescription("转发游戏命令格式")
    val cmdFormat by value("{name} (试图)执行命令 {cmd}")
    @ValueDescription("转发玩家进服格式")
    val joinFormat by value("玩家 {name} 进入了服务器, Xuid为{xuid}")
    @ValueDescription("转发玩家离开格式")
    val leftFormat by value("玩家 {name} 离开了服务器, Xuid为{xuid}")
    @ValueDescription("管理员列表(QQ)")
    val admins by value(mutableListOf(1145141919810))

    fun getServerConfigByID(id: String): ServerConfig? {
        for (server in servers) {
            if (server.serverID == id) return server
        }
        return null
    }
    fun getServerConfigByName(name: String): ServerConfig? {
        for (server in servers) {
            if (server.serverID == name) return server
        }
        return null
    }
}