package nsu.urdin.chatclient;

import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.config.Config;
import nsu.urdin.chatprotocol.dto.Message;

import java.io.*;
import java.net.Socket;

@Slf4j
public class ChatClient {
    private Config config;
    private Socket clientSocket;
    private ObjectOutputStream out;
    private ObjectInputStream in;

    public ChatClient() {
        this.config = new Config();
        startConnection(config.getServerHost(), config.getServerPort());
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
        Message dto = new Message(msg);
        out.writeObject(dto);
    }

    @SneakyThrows
    public void stopConnection() {
        in.close();
        out.close();
        clientSocket.close();
    }

    @SneakyThrows
    public static void main(String[] args) {
        ChatClient app = new ChatClient();

        while (true) {
            app.sendMessage("Hello World");
            Thread.sleep(1000);
        }
    }
}