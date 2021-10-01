package com.jasonzyt.mirai.bdswebsocket

import net.mamoe.mirai.console.data.*
import kotlinx.serialization.*

@Serializable
data class GameInfo (
    var name: String = "",
    var xuid: Long = 0,
    var set: Boolean = false
)

object PluginData : AutoSavePluginData("data") {
    var gameInfo by value<MutableMap<Long, GameInfo>>()
}
