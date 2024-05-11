package nsu.urdin.chatclient;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.config.Config;
import nsu.urdin.chatclient.exception.ConnectionException;
import nsu.urdin.chatclient.exception.RequestException;
import nsu.urdin.chatprotocol.dto.ErrorResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.dto.request.*;

import java.io.*;
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

    private void startConnection(String host, int port) throws ConnectionException {
        try {
            this.clientSocket = new Socket(host, port);
            out = new ObjectOutputStream(clientSocket.getOutputStream());
            in = new ObjectInputStream(clientSocket.getInputStream());
        } catch (IOException e) {
            log.error("Failed to connect to server {}:{}", host, port, e);
            throw new ConnectionException(e.getMessage());
        }

        log.info("Connected to server {}:{}", host, port);
    }

    private SuccessResponse doRequest(RequestBase requestDto) throws RequestException {
        sendRequest(requestDto);

        log.debug("Sent request: {}", requestDto);

        ResponseBase responseDto = receiveResponse(requestDto);

        if (responseDto instanceof ErrorResponse) {
            throw new RequestException(((ErrorResponse) responseDto).getMessage());
        }

        if (responseDto instanceof SuccessResponse) {
            return (SuccessResponse) responseDto;
        }

        log.error("Unknown response type received from server: {}", responseDto);
        throw new RequestException("Unknown response type received from server");
    }

    private void sendRequest(RequestBase requestDto) {
        try {
            out.writeObject(requestDto);
            out.flush();
        } catch (IOException e) {
            log.error("Failed to send request {}", requestDto, e);
            throw new RuntimeException(e);
        }
    }

    private ResponseBase receiveResponse(RequestBase requestDto) {
        ResponseBase responseDto;
        try {
            responseDto = (ResponseBase) in.readObject();
        } catch (IOException | ClassNotFoundException e) {
            log.error("Failed to receive response for request {}", requestDto, e);
            throw new RuntimeException(e);
        }
        log.debug("Received response: {}", responseDto);
        return responseDto;
    }

    public void register(String serverHost, int port, String username, String password)
            throws RequestException, ConnectionException {
        startConnection(serverHost, port);
        RegisterRequest dto = new RegisterRequest(username, password);
        doRequest(dto);
    }

    public void login(String serverHost, int port, String username, String password)
            throws RequestException, ConnectionException {
        startConnection(serverHost, port);
        LoginRequest dto = new LoginRequest(username, password);
        doRequest(dto);
    }

    public void logout() {
        LogoutRequest dto = new LogoutRequest();
        try {
            doRequest(dto);
        } catch (RequestException e) {
            log.error("Logout request failed", e);
            throw new RuntimeException(e);
        }
        stopConnection();
    }

    public void sendMessage(String msg) throws RequestException {
        SendMessageRequest dto = new SendMessageRequest(msg);
        doRequest(dto);
    }

    public UsersListSuccessResponse getUsersList() throws RequestException {
        UsersListRequest dto = new UsersListRequest();
        ResponseBase replyDto = doRequest(dto);
        if (replyDto instanceof UsersListSuccessResponse) {
            return (UsersListSuccessResponse) doRequest(dto);
        }

        log.error("getUsersList: Wrong response type received from server: {}", replyDto);
        throw new RequestException("Wrong response type received from server");
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