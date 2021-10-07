package com.jasonzyt.mirai.bdswebsocket.command

import com.jasonzyt.mirai.bdswebsocket.PluginData
import com.mojang.brigadier.Command
import com.mojang.brigadier.CommandDispatcher
import com.mojang.brigadier.ParseResults
import com.mojang.brigadier.arguments.ArgumentType
import com.mojang.brigadier.arguments.LongArgumentType
import com.mojang.brigadier.builder.LiteralArgumentBuilder
import com.mojang.brigadier.builder.LiteralArgumentBuilder.literal
import com.mojang.brigadier.builder.RequiredArgumentBuilder
import com.mojang.brigadier.builder.RequiredArgumentBuilder.argument
import com.mojang.brigadier.context.CommandContext
import com.mojang.brigadier.tree.LiteralCommandNode
import net.mamoe.mirai.contact.Contact
import net.mamoe.mirai.contact.User
import kotlin.reflect.full.functions

data class CommandSource(val src: User, val contact: Contact?)

class CommandRoot(val name: String, val desc: String) {
    var literal: LiteralArgumentBuilder<CommandSource> = literal(name)
    var node: LiteralCommandNode<CommandSource>? = null

    fun overload(cmd: (CommandContext<CommandSource>) -> Int) {
        literal.executes(Command(cmd))
    }

    fun overload(argNames: List<String>, argTypes: List<ArgumentType<*>>,
                 cmd: ((CommandContext<CommandSource>) -> Int)? = null) {
        var last: RequiredArgumentBuilder<CommandSource, *> =
            argument(argNames[argNames.size - 1], argTypes[argTypes.size - 1])
        var first: RequiredArgumentBuilder<CommandSource, *> = last
        if (cmd != null) last.executes(Command(cmd))
        for (i in argNames.size - 2 downTo 0) {
            first = argument(argNames[i], argTypes[i])
            first.then(last)
            last = first
        }
        literal.then(first)
    }
}

object CommandUtil {
    var commands = mutableMapOf<String, CommandRoot>()
    private var dispatcher = CommandDispatcher<CommandSource>()

    fun getRootUsage(): String {
        var result = ""
        for (cmd in commands)
            result += "+ ${cmd.key} - ${cmd.value.desc}\n"
        return result
    }

    fun getUsage(cmd: String, src: CommandSource): Array<String>? {
        return dispatcher.getAllUsage(commands[cmd]?.node, src, true)
    }

    fun parse(text: String, src: CommandSource): ParseResults<CommandSource>? {
        return dispatcher.parse(text, src)
    }

    fun execute(text: String, src: CommandSource): Int {
        return dispatcher.execute(parse(text, src))
    }

    fun execute(results: ParseResults<CommandSource>): Int {
        return dispatcher.execute(results)
    }

    fun register(root: CommandRoot) {
        commands[root.name] = root
        root.node = dispatcher.register(root.literal)
    }
}

object CommandRegistry {
    val commandNames = hashMapOf(
        "Help" to "help",
        "BindXbox" to "绑定",
        "GetXboxID" to "查ID",
        "GetQQ" to "查QQ",
        "SelfAddWhiteList" to "自助加白",
        "WhiteList" to "白名单"
    )
    private val commandDesc = hashMapOf(
        "Help" to "获取帮助信息",
        "BindXbox" to "绑定XboxID",
        "GetXboxID" to "查询某人的XboxID",
        "GetQQ" to "查XboxID对应的QQ号",
        "SelfAddWhiteList" to "自助添加白名单",
        "WhiteList" to "白名单操作"
    )

    fun registerAll() {
        val clazz = CommandRegistry::class
        val ignore = listOf("registerAll", "equals", "hashCode", "toString")
        for (func in clazz.functions) {
            if (ignore.indexOf(func.name) == -1) {
                func.call(this)
            }
        }
    }

    fun registerHelp() {
        val root = CommandRoot(commandNames["Help"]!!, commandDesc["Help"]!!)
        root.overload { return@overload 0 }
        root.overload(listOf("Command"),
            listOf(EnumArgumentType.enumArg(commandNames.values.toList()))) { return@overload 1 }
        CommandUtil.register(root)
    }

    fun registerGetXboxID() {
        val root = CommandRoot(commandNames["GetXboxID"]!!, commandDesc["GetXboxID"]!!)
        root.overload(listOf("Target"), listOf(AtArgumentType.at())) { return@overload 2 }
        root.overload(listOf("TargetQQ"), listOf(LongArgumentType.longArg())) { return@overload 3 }
        CommandUtil.register(root)
    }
}

object CommandCallbacks {
    suspend fun getHelp(context: CommandContext<CommandSource>) {
        val src = context.source
        src.contact?.sendMessage(CommandUtil.getRootUsage())
    }
    suspend fun getHelpOfCommand(context: CommandContext<CommandSource>) {
        val idx = context.getArgument("Command", Int::class.java)
        val src = context.source
        val cmd = CommandRegistry.commandNames.values.toList()[idx]
        val root = CommandUtil.commands[cmd]
        if (root != null) {
            val content = CommandUtil.getUsage(cmd, src)
            var result = "${root.name} - ${root.desc}\n"
            if (content != null) {
                for (usage in content) {
                    result += "- ${root.name} $usage\n"
                }
                result.dropLast(1)
                src.contact?.sendMessage(result)
                return
            }
        }
        src.contact?.sendMessage("找不到此命令")
    }

    suspend fun getXboxIDAt(context: CommandContext<CommandSource>) {
        val qq = context.getArgument("Target", Long::class.java)
        val res = PluginData.gameInfo[qq]
        val src = context.source
        if (res == null) {
            src.contact?.sendMessage("该用户还未绑定XboxID")
            return
        }
        src.contact?.sendMessage("QQ: $qq\nXbox: ${res.name}\nXuid: ${res.xuid}")
    }
    suspend fun getXboxIDQQ(context: CommandContext<CommandSource>) {
        val qq = context.getArgument("TargetQQ", Long::class.java)
        val res = PluginData.gameInfo[qq]
        val src = context.source
        if (res == null) {
            src.contact?.sendMessage("该用户还未绑定XboxID")
            return
        }
        src.contact?.sendMessage("QQ: $qq\nXbox: ${res.name}\nXuid: ${res.xuid}")
    }
}