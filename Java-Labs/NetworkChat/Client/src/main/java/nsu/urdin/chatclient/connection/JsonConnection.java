package nsu.urdin.chatclient.connection;

import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.extern.slf4j.Slf4j;

import java.io.*;
import java.net.Socket;

@Slf4j
public class JsonConnection extends Connection {

    private BufferedReader in;
    private BufferedWriter out;

    @Override
    protected void createStreams(Socket socket) throws IOException {
        out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
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
        ObjectMapper mapper = new ObjectMapper();
        String json = in.readLine();
        log.debug("Reading JSON object: {}", json);
        String className = mapper.readTree(json).get("className").textValue();
        return mapper.readValue(json, Class.forName(className));
    }

    @Override
    protected void writeObject(Object object) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        String json = mapper.writeValueAsString(object);
        log.debug("Writing JSON object: {}", json);
        out.write(json);
        out.write("\n");
        out.flush();
    }
}
