package nsu.urdin.chatclient.request;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.connection.Connection;
import nsu.urdin.chatclient.exception.ConnectionTimeoutException;
import nsu.urdin.chatclient.exception.RequestException;
import nsu.urdin.chatprotocol.dto.request.*;
import nsu.urdin.chatprotocol.dto.response.*;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

@Slf4j
public class RequestController {

    private static final long REQUEST_TIMEOUT_MS = 1000;

    Connection connection;

    public RequestController(Connection connection) {
        this.connection = connection;
    }

    private SuccessResponse doRequest(RequestBase requestDto) throws RequestException {
        log.debug("Sending request... {}", requestDto);
        ResponseBase responseDto;
        try {
            responseDto = (ResponseBase) connection.sendAndReceive(
                    requestDto, ResponseBase.class, REQUEST_TIMEOUT_MS, TimeUnit.MILLISECONDS);
        } catch (IOException | ConnectionTimeoutException e) {
            throw new RequestException(e.getMessage());
        }
        log.debug("Received response: {}", responseDto);

        if (responseDto instanceof ErrorResponse) {
            throw new RequestException(((ErrorResponse) responseDto).getMessage());
        }

        if (responseDto instanceof SuccessResponse) {
            return (SuccessResponse) responseDto;
        }

        log.error("Unknown response type received from server: {}", responseDto);
        throw new RequestException("Unknown response type received from server");
    }

    public void register(String username, String password)
            throws RequestException {
        RegisterRequest dto = new RegisterRequest(username, password);
        doRequest(dto);
    }

    public void login(String username, String password)
            throws RequestException {
        LoginRequest dto = new LoginRequest(username, password);
        doRequest(dto);
    }

    public void logout() throws RequestException {
        LogoutRequest dto = new LogoutRequest();
        doRequest(dto);
    }

    public void sendMessage(String msg) throws RequestException {
        SendMessageRequest dto = new SendMessageRequest(msg);
        doRequest(dto);
    }

    public UsersListSuccessResponse getUsersList() throws RequestException {
        UsersListRequest dto = new UsersListRequest();
        ResponseBase replyDto = doRequest(dto);
        if (replyDto instanceof UsersListSuccessResponse) {
            return (UsersListSuccessResponse) replyDto;
        }

        log.error("getUsersList: Wrong response type: {}", replyDto);
        throw new RequestException("Wrong response type");
    }

    public ChatHistorySuccessResponse getChatHistory() throws RequestException {
        ChatHistoryRequest dto = new ChatHistoryRequest();
        ResponseBase replyDto = doRequest(dto);
        if (replyDto instanceof ChatHistorySuccessResponse) {
            return (ChatHistorySuccessResponse) replyDto;
        }

        log.error("getChatHistory: Wrong response type: {}", replyDto);
        throw new RequestException("Wrong response type");
    }

}
