package com.jasonzyt.mirai.bdswebsocket.ws

import com.google.gson.*
import com.jasonzyt.mirai.bdswebsocket.*
import com.jasonzyt.mirai.bdswebsocket.utils.AESCrypt

class RawMessage(
    val data: String = "",
    val encrypted: Boolean = true,
    val mode: String = "AES/CBC/PKCS5Padding"
) {

    companion object {
        fun fromJson(json: String): RawMessage? {
            try {
                return Gson().fromJson(json, RawMessage::class.java)
            }
            catch (e: JsonSyntaxException) {
                PluginMain.logger.error(e)
            }
            return null
        }
    }

    override fun toString(): String {
        return Gson().toJson(this)
    }
}

class Message {
    val EVENT_GAMECHAT = "gameChat"
    val EVENT_GAMECMD  = "gameCmd"
    val EVENT_QQCHAT   = "QQChat"
    val EVENT_QQCMD    = "QQCmd"
    val EVENT_JOIN     = "join"
    val EVENT_LEFT     = "left"
    val TYPE_PLAYER_LIST = "getPlayerList"
    val TYPE_STATS_QUERY = "statsQuery"
    val TYPE_XUIDDB_GET  = "getXuid"

    var id = ""
    var event = ""
    var type = ""
    class Data {
        var isFakePlayer: Boolean? = null
        var xuid: Long? = null
        var nick: String? = null
        var name: String? = null
        var msg: String?  = null
        var cmd  = ""
        var success = false
        var reason  = ""
    }
    var data = Data()

    fun encrypt(key: String, iv: String): String {
        val text = this.toString()
        return AESCrypt.encrypt(text, key, iv)
    }

    companion object {
        fun fromJson(json: String): Message? {
            return Gson().fromJson(json, Message::class.java)
        }
    }

    override fun toString(): String {
        return Gson().toJson(this)
    }
}