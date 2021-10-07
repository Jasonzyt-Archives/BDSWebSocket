package com.jasonzyt.mirai.bdswebsocket.ws

import com.jasonzyt.mirai.bdswebsocket.*
import org.java_websocket.framing.CloseFrame
import org.java_websocket.handshake.ServerHandshake
import java.net.ConnectException
import java.net.URI
import java.util.*

class AutoConnectWebSocket : TimerTask() {
    override fun run() {
        for (cli in clients) {
            try {
                PluginMain.logger.info("Try connect")
                if (!cli.isOpen)
                    cli.connect()
            }
            catch (e: ConnectException) {}
        }
    }
}
class WebSocketClient(private var serverUri: URI?) : org.java_websocket.client.WebSocketClient(serverUri) {
    fun send(rawMessage: RawMessage) {
        send(rawMessage.toString())
    }

    override fun onOpen(handshakedata: ServerHandshake?) {
        PluginMain.logger.info("[Mirai-BDSWS][WebSocket] WebSocket server connection opened! URI: " + serverUri.toString())
    }

    override fun onMessage(message: String?) {

    }

    override fun onClose(code: Int, reason: String?, remote: Boolean) {
        if (code != CloseFrame.ABNORMAL_CLOSE) {
            PluginMain.logger.warning(
                "[Mirai-BDSWS][WebSocket] WebSocket server connection abnormal closed! URI: "
                    + serverUri.toString() + " Reason: " + reason
            )
            return
        }
        PluginMain.logger.info("[Mirai-BDSWS][WebSocket] WebSocket server connection closed! URI: " + serverUri.toString())
    }

    override fun onError(ex: Exception?) {
        PluginMain.logger.error(ex)
    }
}

var clients = mutableListOf<WebSocketClient>()