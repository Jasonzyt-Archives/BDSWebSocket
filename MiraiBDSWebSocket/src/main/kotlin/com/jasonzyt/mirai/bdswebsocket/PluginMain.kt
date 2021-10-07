package com.jasonzyt.mirai.bdswebsocket

import com.jasonzyt.mirai.bdswebsocket.command.CommandCallbacks
import com.jasonzyt.mirai.bdswebsocket.command.CommandRegistry
import com.jasonzyt.mirai.bdswebsocket.command.CommandSource
import com.jasonzyt.mirai.bdswebsocket.command.CommandUtil
import com.jasonzyt.mirai.bdswebsocket.ws.AutoConnectWebSocket
import com.jasonzyt.mirai.bdswebsocket.ws.WebSocketClient
import com.jasonzyt.mirai.bdswebsocket.ws.clients
import com.mojang.brigadier.exceptions.CommandSyntaxException
import net.mamoe.mirai.console.plugin.jvm.JvmPluginDescription
import net.mamoe.mirai.console.plugin.jvm.KotlinPlugin
import net.mamoe.mirai.console.util.ConsoleExperimentalApi
import net.mamoe.mirai.event.GlobalEventChannel
import net.mamoe.mirai.event.Listener
import net.mamoe.mirai.event.events.MessageEvent
import java.net.ConnectException
import java.net.URI
import java.util.*


// 定义主类方法 2, 使用 `JvmPluginDescription.loadFromResource()` 从 resources/plugin.yml 加载

object PluginMain : KotlinPlugin(
    @OptIn(ConsoleExperimentalApi::class)
    JvmPluginDescription.loadFromResource()
) {
    private lateinit var messageListener: Listener<MessageEvent>

    private fun onMessageListener() {
        val scope = GlobalEventChannel.parentScope(this)
        messageListener = scope.subscribeAlways {
            var check = false
            if (subject.id == sender.id) check = true
            for (ser in PluginSettings.servers) {
                val res = ser.targetGroups.indexOf(subject.id)
                if (res >= 0) check = true
            }
            try {
                if (check) {
                    val data = CommandUtil.parse(message.serializeToMiraiCode(), CommandSource(sender, subject))
                    if (data != null) {
                        val context = data.context.build("")
                        when (CommandUtil.execute(data)) {
                            0 -> CommandCallbacks.getHelp(context)
                            1 -> CommandCallbacks.getHelpOfCommand(context)
                            2 -> CommandCallbacks.getXboxIDAt(context)
                            3 -> CommandCallbacks.getXboxIDQQ(context)
                            else -> {
                            }
                        }
                    } else {

                    }
                }
            }
            catch (e: IllegalArgumentException) {
                subject.sendMessage(e.message!!)
            }
            catch (e: CommandSyntaxException) {
                subject.sendMessage(e.message!!)
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
        try {
            for (ser in PluginSettings.servers)
                clients.add(WebSocketClient(URI(ser.wsUri)))
        }
        catch (e: ConnectException) {}
        Timer().schedule(AutoConnectWebSocket(), Date(), 5000)
        onMessageListener()
        CommandRegistry.registerAll()
    }

    override fun onDisable() {
        messageListener.complete()
    }
}
