package nsu.urdin.chatclient.gui.controller;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.scene.text.Text;
import javafx.stage.Stage;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.ChatClient;

import java.io.IOException;

@Slf4j
public class LoginController {
    @Setter
    private ChatClient chatClient;

    @FXML
    private Text errorMessageText;

    @FXML
    private Button loginButton;

    @FXML
    private TextField passwordInputField;

    @FXML
    private Button registerButton;

    @FXML
    private TextField serverIPInputField;

    @FXML
    private TextField serverPortInputField;

    @FXML
    private TextField usernameInputField;

    @FXML
    void initialize() {
        errorMessageText.setText("");
    }

    @FXML
    void onLoginButtonPress(ActionEvent event) {
        ChatController.Load((Stage) loginButton.getScene().getWindow(), chatClient);
    }

    @FXML
    void onPasswordInput(ActionEvent event) {

    }

    @FXML
    void onRegisterButtonPress(ActionEvent event) {
        ChatController.Load((Stage) loginButton.getScene().getWindow(), chatClient);
    }

    @FXML
    void onServerIPInput(ActionEvent event) {

    }

    @FXML
    void onServerPortInput(ActionEvent event) {

    }

    @FXML
    void onUsernameInput(ActionEvent event) {

    }

    public static void Load(Stage stage, ChatClient chatClient) {
        FXMLLoader fxmlLoader = new FXMLLoader(ChatController.class.getResource("/login-view.fxml"));

        Parent root;
        try {
            root = fxmlLoader.load();
        } catch (IOException e) {
            log.error("Failed to load login-view.fxml", e);
            throw new RuntimeException(e);
        }

        LoginController loginController = fxmlLoader.getController();
        loginController.setChatClient(chatClient);

        Scene scene = new Scene(root, 500, 400);
        stage.setTitle("Login");
        stage.setScene(scene);
    }
}

