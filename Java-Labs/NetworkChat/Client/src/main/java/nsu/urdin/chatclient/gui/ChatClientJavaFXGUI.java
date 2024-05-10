package nsu.urdin.chatclient.gui;

import javafx.application.Application;
import javafx.stage.Stage;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.ChatClient;
import nsu.urdin.chatclient.gui.controller.LoginController;

@Slf4j
public class ChatClientJavaFXGUI extends Application {

    @Override
    public void start(Stage primaryStage) {

        LoginController.Load(primaryStage, ChatClient.getInstance());
        primaryStage.show();
    }
}
