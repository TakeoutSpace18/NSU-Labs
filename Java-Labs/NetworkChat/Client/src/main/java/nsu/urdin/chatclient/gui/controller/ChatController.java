package nsu.urdin.chatclient.gui.controller;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.ChatClient;
import nsu.urdin.chatclient.ChatEventListener;
import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatprotocol.entity.User;

import java.io.IOException;

@Slf4j
public class ChatController implements ChatEventListener {
    @Setter
    private ChatClient chatClient;

    @FXML
    private Button disconnectButton;

    @FXML
    private TextArea messageTextArea;

    @FXML
    private ListView<String> messagesListView;

    @FXML
    private Label onlineUsersCountLabel;

    @FXML
    private ListView<String> onlineUsersListView;

    @FXML
    private Button sendButton;

    @FXML
    void initialize() {
        Platform.runLater(() -> {
//            try {
//                UsersListSuccessResponse usersResponse = chatClient.getUsersList();
//                onlineUsersCountLabel.setText(String.valueOf(usersResponse.getUsers().size()));
//            } catch (RequestException e) {
//                Alert alert = new Alert(Alert.AlertType.ERROR, "Failed to retrieve users list (" + e.getMessage() + ")", ButtonType.OK);
//                alert.showAndWait();
//            }
        });
    }

    @FXML
    void onDisconnectButtonPress(ActionEvent event) {
        chatClient.logout();
        LoginController.Load((Stage) disconnectButton.getScene().getWindow(), chatClient);
    }

    @FXML
    void onSendButtonPress(ActionEvent event) {

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

        Scene scene = new Scene(root, 1200, 900);
        stage.setTitle("Chat");
        stage.setScene(scene);
    }

    @Override
    public void onNewMessage(Message message) {
        messagesListView.getItems().add(message.getUser() + ": " + message.getText());
    }

    @Override
    public void onUserLogin(User user) {
        onlineUsersListView.getItems().add(user.getName());
        onlineUsersCountLabel.setText(onlineUsersCountLabel.getText() + 1);
    }

    @Override
    public void onUserLogout(User user) {
        onlineUsersListView.getItems().remove(user.getName());
//        onlineUsersCountLabel.setText(onlineUsersCountLabel.getText() - 1);
    }
}
