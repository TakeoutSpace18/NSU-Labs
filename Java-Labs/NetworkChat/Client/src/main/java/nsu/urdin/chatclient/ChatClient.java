package nsu.urdin.chatclient;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.config.Config;

@Slf4j
public class ChatClient {
    private Config config;

    public ChatClient() {
        this.config = new Config();
        System.out.println("Hello Client!");
    }

    public static void main(String[] args) {
        ChatClient app = new ChatClient();
    }
}