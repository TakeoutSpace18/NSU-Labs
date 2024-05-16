package nsu.urdin.chatserver;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.event.EventBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.request.RequestHandlerChain;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

@Slf4j
public class ConnectionSession implements Runnable {
    private static final RequestHandlerChain HANDLER_CHAIN = new RequestHandlerChain();

    private final Socket clientSocket;
    private final ObjectOutputStream out;
    private final ObjectInputStream in;

    private boolean isActive;
    private boolean isLoggedIn;

    @Getter
    private User user;

    public ConnectionSession(Socket clientSocket) throws IOException {
        this.isActive = true;
        this.isLoggedIn = false;
        this.clientSocket = clientSocket;
        out = new ObjectOutputStream(clientSocket.getOutputStream());
        in = new ObjectInputStream(clientSocket.getInputStream());

        log.info("Accepted connection from {}", clientSocket.getRemoteSocketAddress());
    }

    @Override
    public void run() {
        while (isActive) {
            RequestBase requestDto = receiveRequest();
            ResponseBase responseDto = HANDLER_CHAIN.handle(requestDto, this);
            sendResponse(responseDto);
        }
    }

    private void sendResponse(ResponseBase responseDto) {
        try {
            writeObject(responseDto);
        } catch (IOException e) {
            log.error("Failed to send response to {}", clientSocket.getRemoteSocketAddress(), e);
        }
        log.debug("Sent response to {}: {}", clientSocket.getRemoteSocketAddress(), responseDto);
    }

    private RequestBase receiveRequest() {
        RequestBase requestDto;
        try {
            requestDto = (RequestBase) in.readObject();
        } catch (IOException | ClassNotFoundException e) {
            log.error("Failed to receive request", e);
            throw new RuntimeException(e);
        }
        log.debug("Received request from {}: {}", clientSocket.getRemoteSocketAddress(), requestDto);
        return requestDto;
    }

    private synchronized void writeObject(Object obj) throws IOException {
        out.writeObject(obj);
        out.flush();
    }

    public void sendEvent(EventBase event) {
        try {
            writeObject(event);
        } catch (IOException e) {
            log.error("Failed to send event to {}", clientSocket.getRemoteSocketAddress(), e);
        }
    }

    public void stop() {
        isActive = false;
    }

    public boolean isLoggedIn() {
        return isLoggedIn;
    }

    public void login(User user) {
        this.isLoggedIn = true;
        this.user = user;
    }

    public void logout() {
        this.isLoggedIn = false;
        this.user = null;
    }
}
