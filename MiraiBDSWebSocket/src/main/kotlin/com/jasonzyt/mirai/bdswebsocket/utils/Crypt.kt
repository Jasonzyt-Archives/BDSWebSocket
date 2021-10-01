package com.jasonzyt.mirai.bdswebsocket.utils

import com.jasonzyt.mirai.bdswebsocket.PluginMain
import java.security.MessageDigest
import java.security.NoSuchAlgorithmException
import java.util.*
import javax.crypto.Cipher
import javax.crypto.spec.IvParameterSpec
import javax.crypto.spec.SecretKeySpec

object AESCrypt {
    private const val CRYPTMODE = "AES/CBC/PKCS5Padding" // "算法/模式/补码方式"

    @Throws(Exception::class)
    fun encrypt(src: String, sKey: String, cKey:String): String {
        val raw = sKey.toByteArray(charset("utf-8"))
        val sKeySpec = SecretKeySpec(raw, "AES")
        val cipher = Cipher.getInstance(CRYPTMODE)
        val iv = IvParameterSpec(cKey.toByteArray()) // 使用CBC模式，需要一个向量iv，可增加加密算法的强度
        cipher.init(Cipher.ENCRYPT_MODE, sKeySpec, iv)
        val encrypted = cipher.doFinal(src.toByteArray())
        return Base64.getEncoder().encodeToString(encrypted) // 此处使用BASE64做转码功能，同时能起到2次加密的作用。
    }

    fun decrypt(ori: String, sKey: String, cKey: String): String {
        val ins = Cipher.getInstance(CRYPTMODE)
        ins.init(
            2, SecretKeySpec(sKey.toByteArray(), "AES"),
            IvParameterSpec(cKey.toByteArray())
        )
        return String(ins.doFinal(Base64.getDecoder().decode(ori)))
    }
}

object MD5Crypt {
    fun encrypt(src: String): String {
        try {
            val instance = MessageDigest.getInstance("MD5")
            //对字符串加密，返回字节数组
            val digest:ByteArray = instance.digest(src.toByteArray())
            val sb = StringBuffer()
            for (b in digest) {
                //获取低八位有效值
                val i :Int = b.toInt() and 0xff
                //将整数转化为16进制
                var hexString = Integer.toHexString(i)
                if (hexString.length < 2) {
                    //如果是一位的话，补0
                    hexString = "0$hexString"
                }
                sb.append(hexString)
            }
            return sb.toString()
        }
        catch (e: NoSuchAlgorithmException) {
            PluginMain.logger.error(e)
        }
        return ""
    }
}