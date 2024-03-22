package nsu.urdin.tetris;

import nsu.urdin.tetris.controller.TetrisController;

import javax.swing.*;

public class Application {

    public static void main(String[] args) {
        System.setProperty("sun.java2d.uiScale", "2.0");
        SwingUtilities.invokeLater(TetrisController.getInstance()::createAndShowUI);
    }
}