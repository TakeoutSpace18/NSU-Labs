package nsu.urdin.tetris.view;

import lombok.Getter;
import nsu.urdin.tetris.controller.TetrisController;
import nsu.urdin.tetris.model.gameplay.listeners.GameplayStateAdapter;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;

public class JGamePanel {
    @Getter
    private JTetrisFieldPanel tetrisFieldPanel;
    private JPanel contentPanel;
    private JLabel scoreLabel;
    private JLabel levelLabel;
    private JButton pauseButton;
    private JPanel fieldHolderPanel;
    private JPanel sidePanel;

    public JGamePanel() {
        fieldHolderPanel.setBorder(BorderFactory.createLineBorder(JMainFrame.BORDER_COLOR, 4, false));
        sidePanel.setBorder(BorderFactory.createLineBorder(JMainFrame.BORDER_COLOR, 4, false));
        sidePanel.setBackground(JMainFrame.BACKGROUND_COLOR);
        Action pauseAction = new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                TetrisController.getInstance().pause();
            }
        };

        InputMap inputMap = pauseButton.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
        inputMap.put(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), "escape");
        pauseButton.addActionListener(pauseAction);
        pauseButton.getActionMap().put("escape", pauseAction);

        TetrisController.getInstance().setupArrowKeyBindings(tetrisFieldPanel);
    }

    private void createUIComponents() {
        tetrisFieldPanel = new JTetrisFieldPanel();

        TetrisController.getInstance().getGameplayModel().addGameStateListener(new GameplayStateAdapter() {
            @Override
            public void levelChanged(int level) {
                levelLabel.setText("Level: " + level);
            }

            @Override
            public void scoreChanged(int score) {
                scoreLabel.setText("Score: " + score);
            }
        });
    }

}
