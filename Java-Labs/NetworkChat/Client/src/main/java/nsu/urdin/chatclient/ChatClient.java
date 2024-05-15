package nsu.urdin.chatclient;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.config.Config;
import nsu.urdin.chatclient.exception.ConnectionException;
import nsu.urdin.chatclient.exception.RequestException;
import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatprotocol.entity.User;

import java.util.List;

@Slf4j
public class ChatClient {
    private static final ChatClient INSTANCE = new ChatClient();

    @Getter
    private final Config config;
    private final Connection connection;
    RequestController requestController;
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
        this.connection = new Connection();
        this.requestController = new RequestController(connection);
    }

    public void login(String serverHost, int port, String username, String password)
            throws ConnectionException, RequestException {
        if (isLoggedIn) {
            throw new RuntimeException("Already logged in");
        }

        connection.open(serverHost, port);
        requestController.login(username, password);
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
    private void ensureLoggedIn() {
        if (!isLoggedIn) {
            throw new RuntimeException("Not logged in");
        }
    }
}