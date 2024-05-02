package nsu.urdin.chatserver;

import nsu.urdin.chatserver.config.Config;

public class ChatServer {
    private final Config config;

    public ChatServer() {
        config = new Config();
        System.out.println("Hello from server!");
    }

    public static void main(String[] args) {
        ChatServer app = new ChatServer();
    }
}