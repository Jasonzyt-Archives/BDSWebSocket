package com.jasonzyt.mirai.bdswebsocket

import com.jasonzyt.mirai.bdswebsocket.ws.AutoConnectWebSocket
import com.jasonzyt.mirai.bdswebsocket.ws.Message
import com.jasonzyt.mirai.bdswebsocket.ws.WebSocketClient
import com.jasonzyt.mirai.bdswebsocket.ws.clients
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.console.util.ConsoleExperimentalApi
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.Listener
import net.mamoe.mirai.event.events.MessageEvent
import net.mamoe.mirai.message.data.At
import net.mamoe.mirai.message.data.content
import java.net.URI
import java.util.*

// 定义主类方法 2, 使用 `JvmPluginDescription.loadFromResource()` 从 resources/plugin.yml 加载

object PluginMain : KotlinPlugin(
    @OptIn(ConsoleExperimentalApi::class)
    JvmPluginDescription.loadFromResource()
) {
    private lateinit var messageListener: Listener<MessageEvent>
    private var commands = hashMapOf(
        "BindXbox" to "绑定",
        "GetXboxID" to "查ID",
        "GetQQ" to "查QQ",
        "SelfAddWhiteList" to "自助加白",
        "WhiteList" to "白名单"
    )

    private fun onMessageListener() {
        val scope = GlobalEventChannel.parentScope(this)
        messageListener = scope.subscribeAlways<MessageEvent> {
            val messages = message[0].content.split(' ')
            when (messages[0]) {
                commands["BindXbox"] -> {
                    if (PluginSettings.admins.binarySearch(sender.id) > 0) {
                        if (message[1].content[0] == '@') {
                            val at = message.filterIsInstance<At>()
                            if (at.isNotEmpty() && message.size == 3) {
                                PluginData.gameInfo[at[0].target]?.name = message[3].content
                                subject.sendMessage("绑定成功!")
                            }
                            else {
                                subject.sendMessage("命令语法不正确! (" + commands["BindXbox"] + " @Member XboxID)")
                            }
                        }
                        else {
                            if (messages.size == 2) {
                                PluginData.gameInfo[sender.id]?.name = message[3].content
                                subject.sendMessage("绑定成功!")
                            }
                            else {
                                subject.sendMessage("命令语法不正确! (" + commands["BindXbox"] + " XboxID)")
                            }
                        }
                    }
                }

                else -> {
                    for (client in clients) {
                        val msg = Message()
                        msg.event = msg.EVENT_QQCHAT
                        msg.data.msg = message.toString()
                        msg.data.name = PluginData.gameInfo[sender.id]?.name
                    }
                }
            }
        }
    }

    private fun onNudgeListener() {

    }

    override fun onEnable() {
        GlobalEventChannel.exceptionHandler { e -> logger.error(e) }
        PluginSettings.reload()
        PluginData.reload()
        logger.info("Plugin Loaded! Author: Jasonzyt")
        for (ser in PluginSettings.servers)
            clients.add(WebSocketClient(URI(ser.wsUri)))
        Timer().schedule(AutoConnectWebSocket(),Date().time,5000)
        onMessageListener()
    }

    override fun onDisable() {
        messageListener.complete()
    }
}
