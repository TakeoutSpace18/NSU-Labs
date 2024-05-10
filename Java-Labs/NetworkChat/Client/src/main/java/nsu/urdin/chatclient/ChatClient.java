package nsu.urdin.chatclient;

import lombok.Getter;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.config.Config;
import nsu.urdin.chatclient.exception.RequestException;
import nsu.urdin.chatprotocol.dto.ErrorReply;
import nsu.urdin.chatprotocol.dto.ReplyBase;
import nsu.urdin.chatprotocol.dto.SuccessReply;
import nsu.urdin.chatprotocol.dto.request.*;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

@Slf4j
public class ChatClient {
    private static final ChatClient INSTANCE = new ChatClient();

    @Getter
    private final Config config;
    private Socket clientSocket;
    private ObjectOutputStream out;
    private ObjectInputStream in;

    public static ChatClient getInstance() {
        return INSTANCE;
    }

    private ChatClient() {
        this.config = new Config();
    }

    private void startConnection(String host, int port) {
        try {
            this.clientSocket = new Socket(host, port);
            out = new ObjectOutputStream(clientSocket.getOutputStream());
            in = new ObjectInputStream(clientSocket.getInputStream());
        } catch (IOException e) {
            log.error("Failed to connect to server {}:{}", host, port, e);
            throw new RuntimeException(e);
        }

        log.info("Connected to server {}:{}", host, port);
    }

    private SuccessReply sendRequest(RequestBase requestDto) throws RequestException {
        log.debug("Sending request: {}", requestDto);
        try {
            out.writeObject(requestDto);
        } catch (IOException e) {
            log.error("Failed to send request {}", requestDto, e);
            throw new RuntimeException(e);
        }

        ReplyBase replyDto;
        try {
            replyDto = (ReplyBase) in.readObject();
        } catch (IOException | ClassNotFoundException e) {
            log.error("Failed to read reply for request {}", requestDto, e);
            throw new RuntimeException(e);
        }

        if (replyDto instanceof ErrorReply) {
            log.debug("Error reply received: {}", replyDto);
            throw new RequestException(((ErrorReply) replyDto).getMessage());
        }

        if (replyDto instanceof SuccessReply) {
            log.debug("Success reply received: {}", replyDto);
            return (SuccessReply) replyDto;
        }

        log.error("Unknown reply type received from server: {}", replyDto);
        throw new RequestException("Unknown reply type received from server");
    }

    public void register(String serverHost, int port, String username, String password) throws RequestException {
        startConnection(serverHost, port);
        RegisterRequest dto = new RegisterRequest(username, password);
        sendRequest(dto);
    }

    public void login(String serverHost, int port, String username, String password) throws RequestException {
        startConnection(serverHost, port);
        LoginRequest dto = new LoginRequest(username, password);
        sendRequest(dto);
    }

    public void logout() {
        LogoutRequest dto = new LogoutRequest();
        try {
            sendRequest(dto);
        } catch (RequestException e) {
            log.error("Logout request failed", e);
            throw new RuntimeException(e);
        }
        stopConnection();
    }

    public void sendMessage(String msg) throws RequestException {
        SendMessageRequest dto = new SendMessageRequest(msg);
        sendRequest(dto);
    }

    public UsersListSuccessReply getUsersList() throws RequestException {
        UsersListRequest dto = new UsersListRequest();
        ReplyBase replyDto = sendRequest(dto);
        if (replyDto instanceof UsersListSuccessReply) {
            return (UsersListSuccessReply) sendRequest(dto);
        }

        log.error("getUsersList: Wrong reply type received from server: {}", replyDto);
        throw new RequestException("Wrong reply type received from server");
    }

    private void stopConnection() {
        try {
            in.close();
            out.close();
            clientSocket.close();
        } catch (IOException e) {
            log.error("Failed to stop connection", e);
        } finally {
            stopConnectionSilently();
        }
    }

    private void stopConnectionSilently() {
        if (in != null) {
            try {
                in.close();
            }
            catch (IOException ignored) {}
        }
        if (out != null) {
            try {
                out.close();
            }
            catch (IOException ignored) {}
        }
        if (clientSocket != null) {
            try {
                clientSocket.close();
            }
            catch (IOException ignored) {}
        }
    }
}