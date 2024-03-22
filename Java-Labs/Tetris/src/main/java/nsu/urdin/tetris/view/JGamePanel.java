package nsu.urdin.tetris.view;

import lombok.Getter;
import nsu.urdin.tetris.controller.TetrisController;
import nsu.urdin.tetris.model.gameplay.listeners.GameplayStateAdapter;

import javax.swing.*;

public class JGamePanel {
@Getter
    private JTetrisFieldPanel tetrisFieldPanel;
    private JPanel contentPanel;
    private JLabel scoreLabel;
    private JLabel levelLabel;

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
