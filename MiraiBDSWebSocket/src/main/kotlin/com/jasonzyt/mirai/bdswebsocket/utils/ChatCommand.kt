package com.jasonzyt.mirai.bdswebsocket.utils

import com.jasonzyt.mirai.bdswebsocket.PluginMain
import com.mojang.brigadier.Command
import com.mojang.brigadier.CommandDispatcher
import com.mojang.brigadier.arguments.ArgumentType
import com.mojang.brigadier.builder.LiteralArgumentBuilder
import com.mojang.brigadier.builder.LiteralArgumentBuilder.literal
import com.mojang.brigadier.builder.RequiredArgumentBuilder
import com.mojang.brigadier.builder.RequiredArgumentBuilder.argument
import com.mojang.brigadier.tree.LiteralCommandNode


class ChatCommand {
    class CommandRoot(var name: String, var desc: String) {
        var literal: LiteralArgumentBuilder<Long> = literal(name)
        var node: LiteralCommandNode<Long>? = null
        var usage: String = "[$name] - $desc \n"

        fun overload(cmd: Command<Long>) {
            literal.executes(cmd)
        }

        fun overload(argNames: List<String>, argTypes: List<ArgumentType<*>>, cmd: Command<Long>) {
            var last: RequiredArgumentBuilder<Long, *> =
                argument(argNames[argNames.size - 1], argTypes[argTypes.size - 1])
            var first: RequiredArgumentBuilder<Long, *> = last
            usage += "- $name <" + argNames[argNames.size - 1] + argTypes[argTypes.size - 1].toString() + "> "
            last.executes(cmd)
            for (i in argNames.size - 2 downTo 0) {
                first = argument(argNames[i], argTypes[i])
                first.then(last)
                usage += '<'
                last = first
            }
            usage += '\n'
            literal.then(first)
        }
    }

    private var commands = mutableMapOf<String, CommandRoot>()
    private var dispatcher = CommandDispatcher<Long>()

    fun parseCommand(text: String, src: Long) {
        dispatcher.parse(text, src)
    }

    fun newCommand(name: String, desc: String): CommandRoot {
        return CommandRoot(name, desc)
    }

    fun register(name: String) {
        if (commands[name] == null) {
            PluginMain.logger.error("[CMD] Unable to find command [$name]! Failed to register.")
            return
        }
        val node = dispatcher.register(commands[name]?.literal)
        commands[name]?.node = node
    }
}