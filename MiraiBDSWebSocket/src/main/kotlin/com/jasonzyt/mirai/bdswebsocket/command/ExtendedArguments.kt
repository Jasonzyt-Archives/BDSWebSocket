package com.jasonzyt.mirai.bdswebsocket.command

import com.mojang.brigadier.StringReader
import com.mojang.brigadier.arguments.ArgumentType

class NoSpaceStringArgumentType: ArgumentType<String> {
    override fun parse(reader: StringReader): String {
        val start: Int = reader.cursor
        while (reader.canRead() && reader.peek() != ' ') {
            reader.skip()
        }
        return reader.string.substring(start, reader.cursor)
    }

    override fun toString(): String {
        return "string"
    }

    override fun getExamples(): MutableCollection<String> {
        return mutableListOf("word", "[word_with&any*symbols]")
    }

    companion object {
        fun stringWithoutSpace(): NoSpaceStringArgumentType {
            return NoSpaceStringArgumentType()
        }
    }
}

class AtArgumentType: ArgumentType<Long> {
    override fun parse(reader: StringReader?): Long? {
        if (reader != null) {
            val start: Int = reader.cursor
            while (reader.canRead() && reader.peek() != ' ') {
                reader.skip()
            }
            val code = reader.string.substring(start, reader.cursor)
            val list = code.substring(1, code.length - 1).split(':')
            if (list[1] == "at")
                return list[2].toLong()
        }
        return null
    }

    override fun toString(): String {
        return "@at"
    }

    override fun getExamples(): MutableCollection<String> {
        return mutableListOf("@at", "[mirai:at:100000]")
    }

    companion object {
        fun at(): AtArgumentType {
            return AtArgumentType()
        }
    }
}

class EnumArgumentType(private val values: List<String>): ArgumentType<Int> {
    override fun parse(reader: StringReader?): Int {
        if (reader != null) {
            val start: Int = reader.cursor
            while (reader.canRead() && reader.peek() != ' ') {
                reader.skip()
            }
            val str = reader.string.substring(start, reader.cursor)
            return values.indexOf(str)
        }
        return -1
    }

    override fun getExamples(): MutableCollection<String> {
        return values.toMutableList()
    }

    companion object {
        fun enumArg(values: List<String>): EnumArgumentType {
            return EnumArgumentType(values)
        }
    }
}