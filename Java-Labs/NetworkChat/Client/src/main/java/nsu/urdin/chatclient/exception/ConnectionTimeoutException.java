package nsu.urdin.chatclient.exception;

public class ConnectionTimeoutException extends ConnectionException {
    public ConnectionTimeoutException() {
        super("Timeout exceeded");
    }
}
