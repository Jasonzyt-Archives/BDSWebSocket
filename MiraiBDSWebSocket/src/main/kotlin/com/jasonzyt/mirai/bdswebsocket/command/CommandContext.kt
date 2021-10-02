package com.jasonzyt.mirai.bdswebsocket.command

class CommandContext(val res: HashMap<String,ParsedData<*>>) {
    class ParsedData<T>(private val type: Type, private val value: T) {
        enum class Type {
            STRING,INTEGER,LONG,AT,QQ
        }
        fun getString(): T? {
            if (type == Type.STRING) return value
            return null
        }
        fun getInteger(): T? {
            if (type == Type.INTEGER) return value
            return null
        }
        fun getLong(): T? {
            if (type == Type.LONG || type == Type.QQ) return value
            return null
        }
        fun getQQ(): T? {
            if (type == Type.LONG || type == Type.QQ) return value
            return null
        }
        fun getAt(): T? {
            if (type == Type.AT) return value
            return null
        }
    }
}