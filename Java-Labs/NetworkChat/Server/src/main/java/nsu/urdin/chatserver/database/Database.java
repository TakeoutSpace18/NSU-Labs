package nsu.urdin.chatserver.database;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatprotocol.exception.ValidationException;
import nsu.urdin.chatserver.config.Config;
import nsu.urdin.chatserver.exception.DatabaseException;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

@Slf4j
public class Database {
    Connection connection;
    public Database(Config config) throws DatabaseException {
        try {
            connection = DriverManager.getConnection(
                    config.getDatabaseURL(),
                    config.getDatabaseUsername(),
                    config.getDatabasePassword()
            );
        } catch (SQLException e) {
            String msg = String.format("Failed to connect to database (%s)", config.getDatabaseURL());
            log.error(msg, e);
            throw new DatabaseException(msg, e);
        }
    }

    public void addUser(User user, String password) throws DatabaseException {
        try {
            String sql = """
                INSERT INTO users (name, password_hash) VALUES (?, crypt(?, gen_salt('bf', 8)))
                """;
            PreparedStatement preparedStatement = connection.prepareStatement(sql);
            preparedStatement.setString(1, user.getName());
            preparedStatement.setString(2, password);

            int rowsAffected = preparedStatement.executeUpdate();
            if (rowsAffected > 0) {
                log.debug("Added user {} to database", user.getName());
            }
            else {
                throw new SQLException();
            }

        } catch (SQLException e) {
            String msg = "Failed to add user " + user.getName() + " to database";
            log.error(msg, e);
            throw new DatabaseException(msg);
        }
    }

    public Optional<User> findUser(String name) throws DatabaseException {
        try {
            String sql = "SELECT * FROM users WHERE name = ?";
            PreparedStatement preparedStatement = connection.prepareStatement(sql);
            preparedStatement.setString(1, name);

            ResultSet resultSet = preparedStatement.executeQuery();
            if (resultSet.next()) {
                return Optional.of(new User(resultSet.getString("name")));
            } else {
                return Optional.empty();
            }
        } catch (ValidationException | SQLException e) {
            String msg = "Error while finding user " + name + " in database";
            log.error(msg, e);
            throw new DatabaseException(msg);
        }
    }

    public boolean authenticateUser(String name, String password) throws DatabaseException {
        try {
            String sql = """
                SELECT * FROM users WHERE name = ? AND password_hash = crypt(?, password_hash)
                """;
            PreparedStatement preparedStatement = connection.prepareStatement(sql);
            preparedStatement.setString(1, name);
            preparedStatement.setString(2, password);

            ResultSet resultSet = preparedStatement.executeQuery();
            return resultSet.next();
        } catch (SQLException e) {
            String msg = "Error while authenticating user " + name;
            log.error(msg, e);
            throw new DatabaseException(msg);
        }
    }

    public void addMessage(Message message) throws DatabaseException {
        try {
            String sql = """
                INSERT INTO messages (user_id, message_text) VALUES
                ((SELECT users.user_id FROM users WHERE users.name = ?), ?)
                """;
            PreparedStatement preparedStatement = connection.prepareStatement(sql);
            preparedStatement.setString(1, message.getUserName());
            preparedStatement.setString(2, message.getText());

            int rowsAffected = preparedStatement.executeUpdate();
            if (rowsAffected > 0) {
                log.debug("Added message from user {} to database", message.getUserName());
            }
            else {
                throw new SQLException();
            }

        } catch (SQLException e) {
            String msg = "Failed to add message from user " + message.getUserName() + " to database";
            log.error(msg, e);
            throw new DatabaseException(msg);
        }
    }

    public List<Message> getMessages() throws DatabaseException {
        try {
            List<Message> messages = new ArrayList<>();
            String sql = """
                    SELECT u.name, m.message_text
                    FROM messages m JOIN users u
                    ON m.user_id = u.user_id
                    """;
            PreparedStatement preparedStatement = connection.prepareStatement(sql);

            ResultSet resultSet = preparedStatement.executeQuery();
            while (resultSet.next()) {
                String username = resultSet.getString("name");
                String text = resultSet.getString("message_text");
                messages.add(Message.builder()
                        .userName(username)
                        .text(text)
                        .build());
            }
            return messages;
        } catch (SQLException e) {
            String msg = "Failed to get message list from database";
            log.error(msg, e);
            throw new DatabaseException(msg);
        }

    }
}
