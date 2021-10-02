package com.jasonzyt.mirai.bdswebsocket.command

class CommandException(val type: Type): Exception() {
    enum class Type {
        Unknown,
        IntegerArgument,
        LongArgument
    }

    class IntegerArgument(val type: Type, val value: Int, val min: Int, val max: Int) {
        enum class Type { Unknown, TooHigh, TooLow }
    }
    var integerArgument: IntegerArgument? = null

    class LongArgument(val type: Type, val value: Long, val min: Long, val max: Long) {
        enum class Type { Unknown, TooHigh, TooLow }
    }
    var longArgument: LongArgument? = null

    companion object BUILD {
        object Argument {
            fun integerTooHigh(value: Int, min: Int, max: Int): CommandException {
                val e = CommandException(Type.IntegerArgument);
                e.integerArgument = IntegerArgument(IntegerArgument.Type.TooHigh, value, min, max)
                return e
            }
            fun integerTooLow(value: Int, min: Int, max: Int): CommandException {
                val e = CommandException(Type.IntegerArgument);
                e.integerArgument = IntegerArgument(IntegerArgument.Type.TooLow, value, min, max)
                return e
            }
            fun longTooHigh(value: Long, min: Long, max: Long): CommandException {
                val e = CommandException(Type.LongArgument);
                e.longArgument = LongArgument(LongArgument.Type.TooHigh, value, min, max)
                return e
            }
            fun longTooLow(value: Long, min: Long, max: Long): CommandException {
                val e = CommandException(Type.LongArgument);
                e.longArgument = LongArgument(LongArgument.Type.TooLow, value, min, max)
                return e
            }
        }
    }
}