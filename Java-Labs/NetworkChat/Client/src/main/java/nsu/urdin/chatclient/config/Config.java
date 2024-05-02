package nsu.urdin.chatclient.config;

import lombok.extern.slf4j.Slf4j;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

@Slf4j
public class Config {
    private static final String FILENAME = "client.properties";

    private final Properties properties;

    public Config() {
        ClassLoader loader = Thread.currentThread().getContextClassLoader();
        InputStream stream = loader.getResourceAsStream(FILENAME);
        properties = new Properties();

        try {
            properties.load(stream);
        } catch (IOException e) {
            log.error("Can't load config file:\n{}", e.getMessage());
            throw new RuntimeException(e);
        }
    }

    public int getServerPort() {
        return Integer.parseInt(properties.getProperty("ServerPort"));
    }
}
