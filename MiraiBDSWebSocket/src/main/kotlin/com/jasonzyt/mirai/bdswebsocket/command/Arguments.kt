package com.jasonzyt.mirai.bdswebsocket.command

import com.mojang.brigadier.StringReader

abstract class Argument<T> {
    abstract fun parse(reader: StringReader): T?
    abstract fun getType(): CommandContext.ParsedData.Type
}

class IntegerArgument(
    private val minVal: Int = Integer.MIN_VALUE,
    private val maxVal: Int = Integer.MAX_VALUE
): Argument<Int>() {
    @Throws(CommandException::class)
    override fun parse(reader: StringReader): Int {
        val start: Int = reader.cursor
        val result: Int = reader.readInt()
        if (result < minVal) {
            reader.cursor = start
            CommandException.BUILD.Argument.integerTooLow(result, minVal, maxVal)
        }
        if (result > maxVal) {
            reader.cursor = start
            CommandException.BUILD.Argument.integerTooHigh(result, minVal, maxVal)
        }
        return result;
    }

    override fun getType(): CommandContext.ParsedData.Type {
        return CommandContext.ParsedData.Type.INTEGER
    }

    override fun toString(): String {
        return if (minVal == Integer.MIN_VALUE && maxVal == Integer.MAX_VALUE) "int"
               else if (minVal == Integer.MIN_VALUE) "int(<$maxVal)"
               else if (maxVal == Integer.MAX_VALUE) "int(>$minVal)"
               else "int($minVal~$maxVal)"
    }

    companion object {
        fun integer(
            minVal: Int = Integer.MIN_VALUE,
            maxVal: Int = Integer.MAX_VALUE
        ): IntegerArgument {
            return IntegerArgument(minVal, maxVal)
        }
    }
}

class LongArgument(
    private val minVal: Long = Long.MIN_VALUE,
    private val maxVal: Long = Long.MAX_VALUE
): Argument<Long>() {
    @Throws(CommandException::class)
    override fun parse(reader: StringReader): Long {
        val start: Int = reader.cursor
        val result: Long = reader.readLong()
        if (result < minVal) {
            reader.cursor = start
            CommandException.BUILD.Argument.longTooLow(result, minVal, maxVal)
        }
        if (result > maxVal) {
            reader.cursor = start
            CommandException.BUILD.Argument.longTooHigh(result, minVal, maxVal)
        }
        return result;
    }

    override fun getType(): CommandContext.ParsedData.Type {
        return CommandContext.ParsedData.Type.LONG
    }

    override fun toString(): String {
        return if (minVal == Long.MIN_VALUE && maxVal == Long.MAX_VALUE) "long"
        else if (minVal == Long.MIN_VALUE) "long(<$maxVal)"
        else if (maxVal == Long.MAX_VALUE) "long(>$minVal)"
        else "int($minVal~$maxVal)"
    }

    companion object {
        fun long(
            minVal: Long = Long.MIN_VALUE,
            maxVal: Long = Long.MAX_VALUE
        ): LongArgument {
            return LongArgument(minVal, maxVal)
        }
    }
}

class StringArgument(private val type: StringType): Argument<String>() {
    @Throws(CommandException::class)
    override fun parse(reader: StringReader): String {
        return when (type) {
            StringType.GREEDY_PHRASE -> {
                val text = reader.remaining
                reader.cursor = reader.totalLength
                text
            }
            StringType.NO_SPACE_TEXT -> {
                val start: Int = reader.cursor
                while (reader.canRead() && reader.peek() != ' ') {
                    reader.skip()
                }
                reader.string.substring(start, reader.cursor)
            }
            StringType.SINGLE_WORD -> {
                reader.readUnquotedString()
            }
            else -> {
                reader.readString()
            }
        }
    }

    override fun getType(): CommandContext.ParsedData.Type {
        return CommandContext.ParsedData.Type.STRING
    }

    override fun toString(): String {
        return "string"
    }

    enum class StringType {
        SINGLE_WORD, NO_SPACE_TEXT, QUOTABLE_PHRASE, GREEDY_PHRASE
    }

    companion object {
        fun word(): StringArgument {
            return StringArgument(StringType.SINGLE_WORD)
        }

        fun string(): StringArgument {
            return StringArgument(StringType.QUOTABLE_PHRASE)
        }

        fun greedyString(): StringArgument {
            return StringArgument(StringType.GREEDY_PHRASE)
        }

        fun escapeIfRequired(input: String): String {
            for (c in input.toCharArray()) {
                if (!StringReader.isAllowedInUnquotedString(c)) {
                    return escape(input)
                }
            }
            return input
        }

        private fun escape(input: String): String {
            val result = StringBuilder("\"")
            for (element in input) {
                if (element == '\\' || element == '"') {
                    result.append('\\')
                }
                result.append(element)
            }
            result.append("\"")
            return result.toString()
        }
    }
}

class QQArgument: Argument<Long>() {
    @Throws(CommandException::class)
    override fun parse(reader: StringReader): Long? {
        val start: Int = reader.cursor
        val result: Long = reader.readLong()
        if (result < 1000000L) {
            reader.cursor = start
            return null
        }
        else if (result > 1000000000L) {
            reader.cursor = start
            return null
        }
        return result;
    }

    override fun getType(): CommandContext.ParsedData.Type {
        return CommandContext.ParsedData.Type.QQ
    }

    override fun toString(): String {
        return "[QQ]"
    }

    companion object {
        fun qq(): QQArgument {
            return QQArgument()
        }
    }
}

class AtArgument : Argument<Long>() {
    private val strArg = StringArgument(StringArgument.StringType.NO_SPACE_TEXT)
    override fun parse(reader: StringReader): Long? {
        var code = strArg.parse(reader)
        code = code.substring(1, code.length - 2)
        val list = code.split(':')
        if (list[1] == "at") {
            return list[2].toLong()
        }
        return null
    }

    override fun getType(): CommandContext.ParsedData.Type {
        return CommandContext.ParsedData.Type.AT
    }

    override fun toString(): String {
        return "@at"
    }

    companion object {
        fun at(): AtArgument {
            return AtArgument()
        }
    }
}