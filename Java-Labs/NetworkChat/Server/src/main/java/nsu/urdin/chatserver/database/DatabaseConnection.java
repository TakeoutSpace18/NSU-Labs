package nsu.urdin.chatserver.database;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatserver.config.Config;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

@Slf4j
public class DatabaseConnection {
    Connection connection;
    public DatabaseConnection(Config config) throws SQLException {
        connection = DriverManager.getConnection(
                config.getDatabaseURL(),
                config.getDatabaseUsername(),
                config.getDatabasePassword()
        );
        
    }
}
