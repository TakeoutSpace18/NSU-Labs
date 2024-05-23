package nsu.urdin.chatclient.gui.controller;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;
import javafx.stage.Stage;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.ChatClient;
import nsu.urdin.chatclient.event.ServerEventListener;
import nsu.urdin.chatclient.exception.RequestException;
import nsu.urdin.chatprotocol.dto.event.LoginEvent;
import nsu.urdin.chatprotocol.dto.event.LogoutEvent;
import nsu.urdin.chatprotocol.dto.event.MessageEvent;
import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatprotocol.entity.User;

import java.io.IOException;

@Slf4j
public class ChatController implements ServerEventListener {
    @Setter
    private ChatClient chatClient;

    @FXML
    private Button disconnectButton;

    @FXML
    private TextArea messageTextArea;

    @FXML
    private ListView<TextFlow> messagesListView;

    @FXML
    private Label onlineUsersCountLabel;

    @FXML
    private ListView<Text> onlineUsersListView;

    @FXML
    private Button sendButton;

    @FXML
    void initialize() {
        Platform.runLater(() -> {
            updateOnlineUsers();
            updateMessages();
        });
    }

    private Text userToText(User user) {
        Text text = new Text(user.getName());
        text.setFill(Color.rgb(0, 109, 9));
        text.setFont(Font.font("Arial", FontWeight.BOLD, 14));
        return text;
    }

    private TextFlow messageToTextFlow(Message message) {
        Text author = new Text(message.getUserName());
        author.setLayoutX(100);
        author.setFont(Font.font("Arial", FontWeight.BOLD, 14));
        author.setFill(Color.rgb(0, 109, 9));
        Text spacer = new Text("\t");
        Text text = new Text(message.getText());
        return new TextFlow(author, spacer, text);
    }

    private void updateMessages() {
        messagesListView.getItems().clear();
        chatClient.getMessages().stream()
                .map(this::messageToTextFlow)
                .forEach(textFlow -> messagesListView.getItems().add(textFlow));
    }

    void updateOnlineUsers() {
        onlineUsersListView.getItems().clear();
        onlineUsersCountLabel.setText(String.valueOf(chatClient.getOnlineUsersCount()));

        chatClient.getOnlineUsers().stream()
                .map(this::userToText)
                .forEach(text -> onlineUsersListView.getItems().add(text));
    }

    @FXML
    void onDisconnectButtonPress(ActionEvent event) {
        chatClient.logout();
        chatClient.removeServerEventListener(this);
        LoginController.Load((Stage) disconnectButton.getScene().getWindow(), chatClient);
    }

    @FXML
    void onSendButtonPress(ActionEvent event) {
        try {
            if (!messageTextArea.getText().isEmpty()) {
                chatClient.sendMessage(messageTextArea.getText());
                messageTextArea.clear();
            }
        } catch (RequestException e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, e.getMessage(), ButtonType.OK);
            alert.showAndWait();
        }
    }

    public static void Load(Stage stage, ChatClient chatClient) {
        FXMLLoader fxmlLoader = new FXMLLoader(ChatController.class.getResource("/chat-view.fxml"));

        Parent root;
        try {
            root = fxmlLoader.load();
        } catch (IOException e) {
            log.error("Failed to load chat-view.fxml", e);
            throw new RuntimeException(e);
        }

        ChatController chatController = fxmlLoader.getController();
        chatController.setChatClient(chatClient);
        chatClient.addServerEventListener(chatController);

        Scene scene = new Scene(root, 1200, 900);
        stage.setTitle("Chat");
        stage.setScene(scene);
    }

    @Override
    public void onNewMessage(MessageEvent event) {
        Platform.runLater(() -> {
            messagesListView.getItems().add(messageToTextFlow(event.getMessage()));
        });
    }

    @Override
    public void onUserLogin(LoginEvent event) {
        Platform.runLater(() -> {
            onlineUsersListView.getItems().add(userToText(event.getUser()));
            onlineUsersCountLabel.setText(String.valueOf(chatClient.getOnlineUsersCount()));
        });
    }

    @Override
    public void onUserLogout(LogoutEvent event) {
        Platform.runLater(this::updateOnlineUsers);
    }
}
