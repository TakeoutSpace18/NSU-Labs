package nsu.urdin.tetris;

import nsu.urdin.tetris.view.JMainFrame;

import javax.swing.*;

public class Application {

    public static void main(String[] args) {
        SwingUtilities.invokeLater(JMainFrame::new);
    }
}