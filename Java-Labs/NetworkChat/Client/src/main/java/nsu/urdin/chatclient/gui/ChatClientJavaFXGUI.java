package nsu.urdin.chatclient.gui;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatclient.gui.controller.LoginController;

import java.io.IOException;

@Slf4j
public class ChatClientJavaFXGUI extends Application {

    @Override
    public void start(Stage primaryStage) {

        LoginController.Load(primaryStage);
        primaryStage.show();
    }
}
