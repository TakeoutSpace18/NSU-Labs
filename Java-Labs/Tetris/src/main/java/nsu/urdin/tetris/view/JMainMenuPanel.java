package nsu.urdin.tetris.view;

import nsu.urdin.tetris.controller.TetrisController;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class JMainMenuPanel extends JPanel {
    private JPanel contentPanel;
    private JLabel titleLabel;
    private JButton newGameButton;
    private JButton highScoresButton;
    private JButton aboutButton;
    private JButton exitButton;

    public JMainMenuPanel() throws HeadlessException {
        add(contentPanel);
        setVisible(true);
        newGameButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                TetrisController.getInstance().newGame();
            }
        });
    }

}
