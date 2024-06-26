package nsu.urdin.chatclient.event;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.connection.Connection;
import nsu.urdin.chatprotocol.dto.event.EventBase;
import nsu.urdin.chatprotocol.dto.event.LoginEvent;
import nsu.urdin.chatprotocol.dto.event.LogoutEvent;
import nsu.urdin.chatprotocol.dto.event.MessageEvent;

import java.util.ArrayList;
import java.util.List;

@Slf4j
public class ServerEventThread implements AutoCloseable {
    private final Thread thread;

    private List<ServerEventListener> listeners;

    public ServerEventThread(Connection connection) {
        thread = new Thread(() -> {
            while (!Thread.interrupted()) {
                EventBase event = (EventBase) connection.receiveData(EventBase.class);

                log.debug("Handling event: {}", event);
                if (event instanceof MessageEvent messageEvent) {
                    listeners.forEach(l -> l.onNewMessage(messageEvent));
                } else if (event instanceof LogoutEvent logoutEvent) {
                    listeners.forEach(l -> l.onUserLogout(logoutEvent));
                } else if (event instanceof LoginEvent loginEvent) {
                    listeners.forEach(l -> l.onUserLogin(loginEvent));
                }
            }
        });
        thread.setName("Server Event Thread");
        thread.setDaemon(true);
        thread.start();

        listeners = new ArrayList<>();
    }

    public synchronized void addListener(ServerEventListener listener) {
        listeners.add(listener);
    }

    public synchronized void removeListener(ServerEventListener listener) {
        listeners.remove(listener);
    }

    @Override
    public void close() {
        thread.interrupt();
    }
}
