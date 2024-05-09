package nsu.urdin.chatserver;

import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.request.SendMessageRequest;

import java.io.*;
import java.net.Socket;

@Slf4j
public class Session implements Runnable {
    private final Socket clientSocket;
    private ObjectOutputStream out;
    private ObjectInputStream in;

    @SneakyThrows
    public Session(Socket clientSocket) {
        this.clientSocket = clientSocket;
        out = new ObjectOutputStream(clientSocket.getOutputStream());
        in = new ObjectInputStream(clientSocket.getInputStream());

        log.info("Received connection from {}", clientSocket.getRemoteSocketAddress());
    }

    @SneakyThrows
    @Override
    public void run() {
        while (true) {
            SendMessageRequest dto = (SendMessageRequest) in.readObject();
            log.info("Message received: {}", dto);
        }
    }
}
