package nsu.urdin;

import nsu.urdin.tetris.view.MainFrame;

import javax.swing.*;

public class Application {

    public static void main(String[] args) {
        SwingUtilities.invokeLater(MainFrame::new);
    }
}