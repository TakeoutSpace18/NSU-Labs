package nsu.urdin.tetris.view;

import nsu.urdin.tetris.controller.TetrisController;
import nsu.urdin.tetris.model.TetrisFieldImpl;

import javax.swing.*;

public class JMainFrame extends JFrame {

    private static final String WINDOW_NAME = "Tetris";

    public JMainFrame() {
        super(WINDOW_NAME);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        JTetrisField field = new JTetrisField(TetrisFieldImpl.DIMENSIONS);
        TetrisController.getInstance().getField().addListener(field);
        TetrisController.getInstance().scheduleModelUpdate();

        getContentPane().add(field);

        pack();
        setVisible(true);
    }
}