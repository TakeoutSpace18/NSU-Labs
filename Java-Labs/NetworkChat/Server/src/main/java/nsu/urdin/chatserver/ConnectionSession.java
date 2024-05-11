package nsu.urdin.chatserver;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatserver.request.RequestHandlerChain;

import java.io.*;
import java.net.Socket;

@Slf4j
public class ConnectionSession implements Runnable {
    private static final RequestHandlerChain HANDLER_CHAIN = new RequestHandlerChain();

    private final Socket clientSocket;
    private ObjectOutputStream out;
    private ObjectInputStream in;

    public ConnectionSession(Socket clientSocket) throws IOException {
        this.clientSocket = clientSocket;
        out = new ObjectOutputStream(clientSocket.getOutputStream());
        in = new ObjectInputStream(clientSocket.getInputStream());

        log.info("Received connection from {}", clientSocket.getRemoteSocketAddress());
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            RequestBase requestDto = receiveRequest();
            ResponseBase responseDto = HANDLER_CHAIN.handle(requestDto);
            sendResponse(responseDto);
        }
    }

    private void sendResponse(ResponseBase responseDto) {
        try {
            out.writeObject(responseDto);
            out.flush();
        } catch (IOException e) {
            log.error("Failed to send response", e);
            throw new RuntimeException(e);
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
}
