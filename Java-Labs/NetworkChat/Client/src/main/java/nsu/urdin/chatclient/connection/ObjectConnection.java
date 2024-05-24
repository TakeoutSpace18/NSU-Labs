package nsu.urdin.chatclient.connection;

import lombok.extern.slf4j.Slf4j;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

@Slf4j
public class ObjectConnection extends Connection {

    private ObjectInputStream in;
    private ObjectOutputStream out;

    @Override
    protected void createStreams(Socket socket) throws IOException {
        out = new ObjectOutputStream(socket.getOutputStream());
        in = new ObjectInputStream(socket.getInputStream());
    }

    @Override
    protected void closeStreams() {
        try {
            out.close();
            in.close();
        } catch (IOException e) {
            log.warn("Failed to close streams. Closing silently...");
        } finally {
            closeStreamsSilently();
        }
    }

    private void closeStreamsSilently() {
        try {
            if (in != null) {
                in.close();
            }
            if (out != null) {
                out.close();
            }
        }
        catch (IOException ignored) {}
    }


    @Override
    protected Object readObject() throws IOException, ClassNotFoundException {
        return in.readObject();
    }

    @Override
    protected void writeObject(Object object) throws IOException {
        out.writeObject(object);
        out.flush();
    }
}
