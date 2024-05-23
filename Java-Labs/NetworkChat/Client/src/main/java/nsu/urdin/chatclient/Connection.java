package nsu.urdin.chatclient;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.exception.ConnectionException;
import nsu.urdin.chatclient.exception.ConnectionTimeoutException;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

@Slf4j
public class Connection implements AutoCloseable {
    private Socket clientSocket;
    private ObjectOutputStream out;
    private ObjectInputStream in;
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

    public synchronized void sendData(Object data) throws IOException {
        out.writeObject(data);
        out.flush();
        log.debug("Written object to socket: {}", data);
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
        while (!Thread.interrupted() || !clientSocket.isClosed()) {
            received = readObject();
            lock.lock();
            receivedCondition.signalAll();
            log.debug("signalled all waiters");
            lock.unlock();
        }
    }

    public void open(String host, int port) throws ConnectionException {
        if (isOpen) {
            throw new ConnectionException("Connection is already open");
        }

        try {
            this.clientSocket = new Socket(host, port);
            out = new ObjectOutputStream(clientSocket.getOutputStream());
            in = new ObjectInputStream(clientSocket.getInputStream());

            socketListener = new Thread(this::runSocketListener);
            socketListener.setName("Socket Listener Thread");
            socketListener.setDaemon(true);
            socketListener.start();
            isOpen = true;

        } catch (IOException e) {
            log.error("Failed to connect to server {}:{}", host, port, e);
            throw new ConnectionException(e.getMessage());
        }

        log.info("Connected to server {}:{}", host, port);
    }

    private Object readObject() {
        Object obj = null;
        try {
            obj = in.readObject();
            log.debug("Read object from socket: {}", obj);
        } catch (IOException | ClassNotFoundException e) {
            if (!clientSocket.isClosed()) {
                log.error("Failed to read object from socket", e);
                throw new RuntimeException(e);
            }
        }
        return obj;
    }

    @Override
    public void close() {
        socketListener.interrupt();

        try {
            in.close();
            out.close();
            clientSocket.close();
        } catch (IOException e) {
            log.error("Failed to close connection", e);
        } finally {
            closeConnectionSilently();
            isOpen = false;
        }
    }

    private void closeConnectionSilently() {
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
