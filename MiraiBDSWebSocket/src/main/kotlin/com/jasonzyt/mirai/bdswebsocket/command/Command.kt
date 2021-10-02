package com.jasonzyt.mirai.bdswebsocket.command

import com.jasonzyt.mirai.bdswebsocket.PluginMain
import com.mojang.brigadier.StringReader
import com.mojang.brigadier.exceptions.CommandSyntaxException

object Command {
    private var commands = hashMapOf<String, CommandRoot>()

    fun register(root: CommandRoot) {
        commands[root.name] = root
    }
    fun parse(text: String): CommandContext? {
        val rootStr = text.split(' ')[0]
        val root = commands[rootStr]
        val res = hashMapOf<String,CommandContext.ParsedData<*>>()
        if (root != null) {
            val reader = StringReader(text.substring(rootStr.length - 1, text.length - 1))
            for (child in root.children) {
                for (i in 0..child.desc.size) {
                    try {
                        res[child.desc[i]] =
                            CommandContext.ParsedData(child.args[i].getType(),child.args[i].parse(reader))
                    }
                    catch (e: CommandException) {
                        res.clear()
                        continue
                    }
                    catch (e: CommandSyntaxException) {
                        res.clear()
                        continue
                    }
                }
            }
            if (res.isNotEmpty())
                return CommandContext(res)
        }
        return null
    }
    fun getSimpleUsage(): String {
        var result = ""
        for (command in commands) {
            result += "+ ${command.value.usage}\n"
        }
        return result.substring(0, result.length - 2)
    }
    fun getUsage(): String {
        var result = ""
        for (command in commands) {
            result += "${command.value.getUsage()}\n"
        }
        return result.substring(0, result.length - 2)
    }
}

class CommandRoot(
    var name: String,
    var desc: String,
    val cb: ((CommandContext) -> Void)? = null
) {
    var usage = "$name - $desc"
    var children = mutableListOf<CommandNode>()

    fun overload(
        args: List<Argument<*>>,
        desc: List<String>,
        cb: ((CommandContext) -> Void)? = null
    ) {
        children.add(CommandNode(name, args, desc, cb))
    }
    @JvmName("getUsage1")
    fun getUsage(): String {
        var result = "$usage\n"
        for (child in children) {
            result += "- ${child.usage}\n"
        }
        return result.substring(0, result.length - 2)
    }
}

class CommandNode(
    val parent: String,
    val args: List<Argument<*>>,
    val desc: List<String>,
    val cb: ((CommandContext) -> Void)? = null
) {
    var usage = "$parent "

    init {
        for (i in 0..desc.size) {
            usage += "<${desc[i]}: ${args[i]}> "
        }
    }
}

fun TestCommand() {
    val root = CommandRoot("test","test")
    root.overload(listOf(IntegerArgument.integer(), AtArgument.at()), listOf("arg1","arg2"))
    Command.register(root)
    val str = "test 192746 [mirai:at:2126931871]"
    val res = Command.parse(str)
    PluginMain.logger.info((res == null).toString())
    PluginMain.logger.info(res?.res?.get("arg1")?.getInteger().toString())
    PluginMain.logger.info(res?.res?.get("arg1")?.getAt().toString())
}