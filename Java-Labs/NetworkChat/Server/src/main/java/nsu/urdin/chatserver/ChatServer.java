package nsu.urdin.chatserver;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatserver.config.Config;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

@Slf4j
public class ChatServer implements Runnable {
    private final Config config;
    private ServerSocket serverSocket;
    private final List<Thread> sessions;

    public ChatServer() {
        config = new Config();
        sessions = new ArrayList<>();
    }

    private void start(int port) {
        log.info("Starting ChatServer on port {}", port);

        try {
            serverSocket = new ServerSocket(port);
        } catch (IllegalArgumentException | SecurityException | IOException e) {
            log.error("Failed to start server", e);
            throw new RuntimeException(e);
        }
    }

    public void stop() {
        log.info("Stopping ChatServer...");

        sessions.forEach(Thread::interrupt);
        Thread.currentThread().interrupt();

        try {
            serverSocket.close();
        } catch (IOException e) {
            log.error("Error while closing serverSocket", e);
        } finally {
            closeSilently(serverSocket);
        }
    }

    @Override
    public void run() {
        start(config.getServerPort());

        while (!Thread.interrupted()) {
            try {
                Socket client = serverSocket.accept();
                Thread sessionThread = new Thread(new ConnectionSession(client));
                sessionThread.start();
                sessions.add(sessionThread);
            } catch (IOException e) {
                log.warn("Failed to accept client connection", e);
            }
        }

        stop();
    }

    private void closeSilently(ServerSocket socket) {
        if (socket != null) {
            try {
                socket.close();
            }
            catch (IOException ignored) {}
        }
    }
}