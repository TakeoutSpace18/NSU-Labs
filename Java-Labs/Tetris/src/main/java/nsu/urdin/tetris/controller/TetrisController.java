package nsu.urdin.tetris.controller;

import lombok.Getter;
import nsu.urdin.tetris.model.listeners.GameStateAdapter;
import nsu.urdin.tetris.view.JMainFrame;
import nsu.urdin.tetris.model.GameModel;
import nsu.urdin.tetris.model.GameModelImpl;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;

public class TetrisController {
    private static final TetrisController INSTANCE = new TetrisController();

    @Getter
    private final GameModel gameModel;
    private JMainFrame mainFrame;
    private Timer modelUpdateTimer;

    private TetrisController() {
        gameModel = new GameModelImpl();
    }

    public void launch() {
        mainFrame = new JMainFrame();

        gameModel.addFieldListener(mainFrame.getTetrisFieldListener());
        gameModel.addGameStateListener(new GameStateAdapter() {
            @Override
            public void speedChanged(int delayBetweenSteps) {
                scheduleModelUpdate(delayBetweenSteps);
            }

            @Override
            public void onGameOver() {
                gameOver();
            }
        });

        setupKeyBindings(mainFrame.getRootPane());
    }

    public static TetrisController getInstance() {
        return INSTANCE;
    }

    public void scheduleModelUpdate(int delay) {
        modelUpdateTimer = new Timer(delay, (ActionEvent e) -> {
            gameModel.nextStep();
        });

        modelUpdateTimer.start();
    }

    public void setupKeyBindings(JComponent component) {
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, 0), "left");
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, 0), "right");
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, 0), "up");
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, 0), "down");

        component.getActionMap().put("left", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameModel.moveLeft();
            }
        });
        component.getActionMap().put("right", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameModel.moveRight();
            }
        });
        component.getActionMap().put("down", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameModel.fastFall();
            }
        });
        component.getActionMap().put("up", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameModel.rotate();
            }
        });
    }

    public void newGame() {
        gameModel.restart();
        mainFrame.showCard("GameCard");
    }

    public void gameOver() {
        modelUpdateTimer.stop();
        mainFrame.showCard("GameOverCard");
        Timer returnToMenu = new Timer(3000, (ActionEvent e) -> {
            mainFrame.showCard("MainMenuCard");
        });
        returnToMenu.setRepeats(false);
        returnToMenu.start();
    }
}
