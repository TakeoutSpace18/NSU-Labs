package nsu.urdin.tetris.view;

import nsu.urdin.tetris.model.GameFieldModel;

import javax.swing.*;

public class MainFrame extends JFrame {

    private static String WINDOW_NAME = "Tetris";

    public MainFrame() {
        super(WINDOW_NAME);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        GameFieldPanel field = new GameFieldPanel(new GameFieldModel());

        getContentPane().add(field);

        setSize(640, 480);
        setVisible(true);
    }
}