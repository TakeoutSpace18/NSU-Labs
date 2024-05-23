package nsu.urdin.chatserver;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.event.EventBase;
import nsu.urdin.chatprotocol.dto.event.LogoutEvent;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.config.Config;
import nsu.urdin.chatserver.database.Database;
import nsu.urdin.chatserver.exception.DatabaseException;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Slf4j
public class ChatServer implements Runnable {
    private static final ChatServer INSTANCE = new ChatServer();

    private final Config config;
    private ServerSocket serverSocket;
    private final List<ConnectionSession> sessions;
    private final ExecutorService threadPool;

    @Getter
    private final Database database;

    public static ChatServer getInstance() {
        return INSTANCE;
    }

    public ChatServer() {
        config = new Config();
        sessions = new ArrayList<>();
        threadPool = Executors.newCachedThreadPool();

        try {
            database = new Database(config);
        } catch (DatabaseException e) {
            throw new RuntimeException(e);
        }
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

        sessions.forEach(ConnectionSession::stop);
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
                ConnectionSession session = new ConnectionSession(client);
                sessions.add(session);
                threadPool.execute(session);
            } catch (IOException e) {
                log.warn("Failed to accept client connection", e);
            }
        }

        stop();
    }

    public void broadcastEvent(EventBase event, ConnectionSession sourceSession, boolean sendToYourself) {
        log.debug("Broadcasting event {} to {} clients...", event, sendToYourself ? sessions.size() : sessions.size() - 1);
        sessions.forEach(session -> {
            if (session.isLoggedIn() && (sendToYourself || session != sourceSession)) {
                session.sendEvent(event);
            }
        });
    }

    private void closeSilently(ServerSocket socket) {
        if (socket != null) {
            try {
                socket.close();
            }
            catch (IOException ignored) {}
        }
    }

    public List<User> getOnlineUsersList() {
        return sessions.stream()
                .filter(ConnectionSession::isLoggedIn)
                .map(ConnectionSession::getUser)
                .toList();
    }

    public void closeSession(ConnectionSession session) {
        log.info("Closing session with {}...", session.getRemoteSocketAddress());


        if (session.isLoggedIn()) {
            broadcastEvent(new LogoutEvent(session.getUser().getName()), session, false);
            session.logout();
        }
        session.stop();
        sessions.remove(session);
    }
}