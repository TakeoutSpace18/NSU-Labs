package nsu.urdin.chatclient;

import lombok.Getter;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.config.Config;
import nsu.urdin.chatprotocol.dto.request.SendMessageRequest;

import java.io.*;
import java.net.Socket;

@Slf4j
public class ChatClient {
    private static final ChatClient INSTANCE = new ChatClient();

    @Getter
    private Config config;
    private Socket clientSocket;
    private ObjectOutputStream out;
    private ObjectInputStream in;

    public static ChatClient getInstance() {
        return INSTANCE;
    }

    private ChatClient() {
        this.config = new Config();
    }

    @SneakyThrows
    public void startConnection(String host, int port) {
        try {
            this.clientSocket = new Socket(host, port);
        } catch (IOException e) {
            log.error("Failed to connect to server {}:{}", host, port, e);
            throw new RuntimeException(e);
        }

        out = new ObjectOutputStream(clientSocket.getOutputStream());
        in = new ObjectInputStream(clientSocket.getInputStream());

        log.info("Connected to server {}:{}", host, port);
    }

    @SneakyThrows
    public void sendMessage(String msg) {
        SendMessageRequest dto = new SendMessageRequest(msg);
        out.writeObject(dto);
    }

    @SneakyThrows
    public void stopConnection() {
        in.close();
        out.close();
        clientSocket.close();
    }
}