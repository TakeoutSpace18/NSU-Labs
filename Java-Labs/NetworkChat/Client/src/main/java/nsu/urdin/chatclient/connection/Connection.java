package nsu.urdin.chatclient.connection;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.exception.ConnectionException;
import nsu.urdin.chatclient.exception.ConnectionTimeoutException;

import java.io.IOException;
import java.net.Socket;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

@Slf4j
public abstract class Connection implements AutoCloseable {
    private Socket socket;
    private Thread socketListener;

    private Object received;

    private final Lock lock;
    private final Condition receivedCondition;

    private boolean isOpen;

    public Connection() {
        lock = new ReentrantLock();
        receivedCondition = lock.newCondition();
        isOpen = false;
    }

    // Blocking receive
    public Object receiveData(Class<?> clazz) {
        lock.lock();
        try {
            do {
                receivedCondition.await();
            } while (received == null || !clazz.isAssignableFrom(received.getClass()));
            return received;
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        } finally {
            lock.unlock();
        }
    }

    // Receive with timeout
    public Object receiveData(Class<?> clazz, long timeout, TimeUnit unit) throws ConnectionTimeoutException {
        lock.lock();
        try {
            do {
                if (!receivedCondition.await(timeout, unit)) {
                    throw new ConnectionTimeoutException();
                }
            } while (received == null || !clazz.isAssignableFrom(received.getClass()));
            return received;
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        } finally {
            lock.unlock();
        }
    }

    public synchronized Object sendAndReceive(Object data, Class<?> receiveType, long timeout, TimeUnit unit)
            throws IOException, ConnectionTimeoutException {
        lock.lock();
        sendData(data);
        try {
            do {
                if (!receivedCondition.await(timeout, unit)) {
                    throw new ConnectionTimeoutException();
                }
            } while (received == null || !receiveType.isAssignableFrom(received.getClass()));
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        } finally {
            lock.unlock();
        }
        return received;
    }

    private void runSocketListener() {
        while (!Thread.interrupted() || !socket.isClosed()) {
            try {
                received = readObject();
            } catch (IOException | ClassNotFoundException e) {
                if (!socket.isClosed()) {
                    log.error("Failed to receive data", e);
                    throw new RuntimeException(e);
                }
            }
            lock.lock();
            receivedCondition.signalAll();
            lock.unlock();
        }
    }

    public void open(String host, int port) throws ConnectionException {
        if (isOpen) {
            throw new ConnectionException("Connection is already open");
        }

        try {
            socket = new Socket(host, port);
            createStreams(socket);

            socketListener = new Thread(this::runSocketListener);
            socketListener.setName("Socket Listener Thread");
            socketListener.setDaemon(true);
            socketListener.start();
            isOpen = true;

        } catch (IllegalArgumentException | IOException e) {
            log.error("Failed to connect to server {}:{}", host, port, e);
            throw new ConnectionException(e.getMessage());
        }

        log.info("Connected to server {}:{}", host, port);
    }

    protected abstract void createStreams(Socket socket) throws IOException;
    protected abstract void closeStreams();
    protected abstract Object readObject() throws IOException, ClassNotFoundException;
    protected abstract void writeObject(Object object) throws IOException;

    public synchronized void sendData(Object data) throws IOException {
        writeObject(data);
    }

    @Override
    public void close() {
        socketListener.interrupt();
        closeStreams();

        try {
            socket.close();
        } catch (IOException e) {
            log.warn("Failed to close socket connection. Closing silently...", e);
        } finally {
            closeConnectionSilently();
            isOpen = false;
        }
    }

    private void closeConnectionSilently() {
        if (socket != null) {
            try {
                socket.close();
            }
            catch (IOException ignored) {}
        }
    }
}
