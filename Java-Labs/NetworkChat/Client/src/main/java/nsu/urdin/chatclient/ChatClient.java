package nsu.urdin.chatclient;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.config.Config;
import nsu.urdin.chatclient.connection.Connection;
import nsu.urdin.chatclient.connection.JsonConnection;
import nsu.urdin.chatclient.connection.ObjectConnection;
import nsu.urdin.chatclient.event.ServerEventListener;
import nsu.urdin.chatclient.event.ServerEventThread;
import nsu.urdin.chatclient.exception.ConnectionException;
import nsu.urdin.chatclient.exception.RequestException;
import nsu.urdin.chatclient.request.RequestController;
import nsu.urdin.chatprotocol.dto.event.LoginEvent;
import nsu.urdin.chatprotocol.dto.event.LogoutEvent;
import nsu.urdin.chatprotocol.dto.event.MessageEvent;
import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatprotocol.entity.User;

import java.util.ArrayList;
import java.util.List;

@Slf4j
public class ChatClient implements ServerEventListener {
    private static final ChatClient INSTANCE = new ChatClient();

    @Getter
    private final Config config;
    private final Connection connection;
    private final RequestController requestController;
    private final ServerEventThread serverEventThread;

    private int onlineUsersCount;
    private List<User> onlineUsers;
    private List<Message> messages;
    private boolean isLoggedIn;

    public static ChatClient getInstance() {
        return INSTANCE;
    }

    private ChatClient() {
        this.isLoggedIn = false;
        this.config = new Config();

        if (config.jsonCommunication()) {
            // Use json to send data
            this.connection = new JsonConnection();
        } else {
            // Use Java object serialization to send data
            this.connection = new ObjectConnection();
        }

        this.requestController = new RequestController(connection);
        this.serverEventThread = new ServerEventThread(connection);
        this.serverEventThread.addListener(this);

        this.onlineUsers = new ArrayList<>();
        this.messages = new ArrayList<>();
    }

    public void login(String serverHost, int port, String username, String password)
            throws ConnectionException, RequestException {
        if (isLoggedIn) {
            throw new RuntimeException("Already logged in");
        }

        connection.open(serverHost, port);
        try {
            requestController.login(username, password);
        } catch (RequestException e) {
            connection.close();
            throw e;
        }
        fetchData();
        isLoggedIn = true;
    }

    public void registerAndLogin(String serverHost, int port, String username, String password)
            throws ConnectionException, RequestException {
        if (isLoggedIn) {
            throw new RuntimeException("Already logged in");
        }
        connection.open(serverHost, port);
        requestController.register(username, password);
        requestController.login(username, password);
        fetchData();
        isLoggedIn = true;
    }

    public void logout() {
        try {
            requestController.logout();
        } catch (RequestException e) {
            log.warn("Logout request failed", e);
        }
        isLoggedIn = false;
        connection.close();
    }

    public int getOnlineUsersCount() {
        ensureLoggedIn();
        return onlineUsersCount;
    }

    public List<User> getOnlineUsers() {
        ensureLoggedIn();
        return onlineUsers;
    }

    public List<Message> getMessages() {
        ensureLoggedIn();
        return messages;
    }

    public void sendMessage(String text) throws RequestException {
        ensureLoggedIn();
        requestController.sendMessage(text);
    }

    private void ensureLoggedIn() {
        if (!isLoggedIn) {
            throw new IllegalStateException("Not logged in");
        }
    }

    private void fetchData() throws RequestException {
        onlineUsers = new ArrayList<>(requestController.getUsersList().getUsers());
        onlineUsersCount = onlineUsers.size();
        messages = new ArrayList<>(requestController.getChatHistory().getMessages());
    }

    @Override
    public void onNewMessage(MessageEvent event) {
        messages.add(event.getMessage());
    }

    @Override
    public void onUserLogin(LoginEvent event) {
        onlineUsers.add(event.getUser());
        onlineUsersCount++;
    }

    @Override
    public void onUserLogout(LogoutEvent event) {
        var userToDelete = onlineUsers.stream().filter(user -> user.getName().equals(event.getName())).findFirst();
        userToDelete.ifPresent(user -> onlineUsers.remove(user));
        onlineUsersCount--;
    }

    public void addServerEventListener(ServerEventListener listener) {
        serverEventThread.addListener(listener);
    }

    public void removeServerEventListener(ServerEventListener listener) {
        serverEventThread.removeListener(listener);
    }
}