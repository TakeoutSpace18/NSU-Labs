package nsu.urdin.tetris.view;

import nsu.urdin.tetris.controller.TetrisController;

import javax.swing.*;
import java.awt.*;

public class JMainMenuPanel {
    private JPanel contentPanel;
    private JLabel titleLabel;
    private JButton newGameButton;
    private JButton highScoresButton;
    private JButton aboutButton;
    private JButton exitButton;

    public JMainMenuPanel() throws HeadlessException {
        newGameButton.addActionListener(actionEvent -> TetrisController.getInstance().newGame());
        highScoresButton.addActionListener(actionEvent -> TetrisController.getInstance().showHighScores());
        aboutButton.addActionListener(actionEvent -> TetrisController.getInstance().about());
        exitButton.addActionListener(actionEvent -> TetrisController.getInstance().exit());
    }

}
