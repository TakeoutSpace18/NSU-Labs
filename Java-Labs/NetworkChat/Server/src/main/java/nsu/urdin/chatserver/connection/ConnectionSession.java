package nsu.urdin.chatserver.connection;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.event.EventBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.response.ResponseBase;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.request.RequestHandlerChain;

import java.io.IOException;
import java.net.Socket;
import java.net.SocketAddress;

@Slf4j
public abstract class ConnectionSession implements Runnable {
    private static final RequestHandlerChain HANDLER_CHAIN = new RequestHandlerChain();

    private final Socket socket;

    private boolean isActive;
    private boolean isLoggedIn;

    @Getter
    private User user;

    public ConnectionSession(Socket socket) throws IOException {
        this.isActive = true;
        this.isLoggedIn = false;
        this.socket = socket;

        createStreams(socket);

        log.info("Accepted connection from {}", socket.getRemoteSocketAddress());
    }

    @Override
    public void run() {
        while (isActive) {
            RequestBase requestDto;
            try {
                requestDto = (RequestBase) readObject();
            } catch (IOException e) {
                ChatServer.getInstance().closeSession(this);
                break;
            } catch (ClassNotFoundException e) {
                throw new RuntimeException("Failed to determine received object class", e);
            }

            log.debug("Received request from {}: {}", socket.getRemoteSocketAddress(), requestDto);

            ResponseBase responseDto = HANDLER_CHAIN.handle(requestDto, this, ChatServer.getInstance().getDatabase());
            sendResponse(responseDto);
        }

        closeConnection();
    }

    private void sendResponse(ResponseBase responseDto) {
        try {
            writeObject(responseDto);
        } catch (IOException e) {
            log.error("Failed to send response to {}", socket.getRemoteSocketAddress(), e);
        }
        log.debug("Sent response to {}: {}", socket.getRemoteSocketAddress(), responseDto);
    }

    public synchronized void sendEvent(EventBase event) {
        try {
            writeObject(event);
        } catch (IOException e) {
            log.error("Failed to send event to {}", socket.getRemoteSocketAddress(), e);
        }
    }

    public void stop() {
        isActive = false;
    }

    public SocketAddress getRemoteSocketAddress() {
        return socket.getRemoteSocketAddress();
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

    private void closeConnection() {
        closeStreams();

        try {
            socket.close();
        } catch (IOException e) {
            log.warn("Failed to close socket. Closing silently...", e);
        } finally {
            closeSocketSilently();
        }
    }

    private void closeSocketSilently() {
        try {
            if (socket != null) {
                socket.close();
            }
        } catch (IOException ignored) {}
    }

    protected abstract void createStreams(Socket socket) throws IOException;

    protected abstract void closeStreams();

    protected abstract Object readObject() throws IOException, ClassNotFoundException;

    protected abstract void writeObject(Object object) throws IOException;

}
