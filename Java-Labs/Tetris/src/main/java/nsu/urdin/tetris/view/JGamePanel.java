package nsu.urdin.tetris.view;

import lombok.Getter;
import nsu.urdin.tetris.controller.TetrisController;
import nsu.urdin.tetris.model.listeners.GameStateAdapter;

import javax.swing.*;

public class JGamePanel {
@Getter
    private JTetrisFieldPanel tetrisFieldPanel;
    private JPanel contentPanel;
    private JLabel scoreLabel;
    private JLabel levelLabel;

    private void createUIComponents() {
        tetrisFieldPanel = new JTetrisFieldPanel();

        TetrisController.getInstance().getGameModel().addGameStateListener(new GameStateAdapter() {
            @Override
            public void levelChanged(int level) {
                levelLabel.setText("Level: " + Integer.toString(level));
            }

            @Override
            public void scoreChanged(int score) {
                scoreLabel.setText("Score: " + Integer.toString(score));
            }
        });
    }

}
