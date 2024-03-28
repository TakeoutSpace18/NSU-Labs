package nsu.urdin.tetris.controller;

import lombok.Getter;
import nsu.urdin.tetris.model.Scoreboard;
import nsu.urdin.tetris.model.exceptions.CantReadScoreboardFileException;
import nsu.urdin.tetris.model.gameplay.GameplayModel;
import nsu.urdin.tetris.model.gameplay.GameplayModelImpl;
import nsu.urdin.tetris.model.gameplay.listeners.GameplayStateAdapter;
import nsu.urdin.tetris.view.JAboutDialog;
import nsu.urdin.tetris.view.JMainFrame;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class TetrisController {
    private static final TetrisController INSTANCE = new TetrisController();

    @Getter
    private GameplayModel gameplayModel;
    @Getter
    private Scoreboard scoreboard;
    private JMainFrame mainFrame;
    private Timer modelUpdateTimer;

    public TetrisController() {
        createGameModel();
        createScoreboardModel();
    }

    public void createAndShowUI() {
        mainFrame = new JMainFrame();
        mainFrame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                exit();
            }
        });

        gameplayModel.addFieldListener(mainFrame.getTetrisFieldListener());
    }

    private void createScoreboardModel() {
        try {
            scoreboard = Scoreboard.readFromFile();
        } catch (CantReadScoreboardFileException e) {
            System.err.println("Failed to load scoreboard file");
            scoreboard = new Scoreboard();
        }
    }

    private void createGameModel() {
        gameplayModel = new GameplayModelImpl();
        gameplayModel.addGameStateListener(new GameplayStateAdapter() {
            @Override
            public void speedChanged(int delayBetweenSteps) {
                scheduleModelUpdate(delayBetweenSteps);
            }

            @Override
            public void onGameOver(int finalScore) {
                gameOver(finalScore);
            }
        });
    }

    public static TetrisController getInstance() {
        return INSTANCE;
    }

    public void scheduleModelUpdate(int delay) {
        if (modelUpdateTimer != null) {
            modelUpdateTimer.stop();
        }

        modelUpdateTimer = new Timer(delay, (ActionEvent e) -> {
            gameplayModel.nextStep();
        });

        modelUpdateTimer.start();
    }

    public void setupArrowKeyBindings(JComponent component) {
        InputMap inputMap = component.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
        inputMap.put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, 0), "left");
        inputMap.put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, 0), "right");
        inputMap.put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, 0), "up");
        inputMap.put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, 0), "down");

        component.getActionMap().put("left", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameplayModel.moveLeft();
            }
        });
        component.getActionMap().put("right", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameplayModel.moveRight();
            }
        });
        component.getActionMap().put("down", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameplayModel.fastFall();
            }
        });
        component.getActionMap().put("up", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                gameplayModel.rotate();
            }
        });
    }

    public void newGame() {
        gameplayModel.restart();
        mainFrame.showCard("GameCard");
    }

    public void gameOver(int finalScore) {
        scoreboard.addEntry(finalScore);
        modelUpdateTimer.stop();
        mainFrame.showCard("GameOverCard");
        Timer returnToMenu = new Timer(3000, (ActionEvent e) -> {
            mainFrame.showCard("MainMenuCard");
        });
        returnToMenu.setRepeats(false);
        returnToMenu.start();
    }

    public void exit() {
        scoreboard.saveToFile();
        System.exit(0);
    }

    public void about() {
        JAboutDialog dialog = new JAboutDialog();
        dialog.pack();
        dialog.setVisible(true);
    }

    public void showHighScores() {
        mainFrame.showCard("HighScoresCard");
    }

    public void showMainMenu() {
        mainFrame.showCard("MainMenuCard");
    }

    public void stopGameAndReturnToMainMenu() {
        mainFrame.hidePauseMenu();
        modelUpdateTimer.stop();
        scoreboard.addEntry(gameplayModel.getCurrentScore());
        mainFrame.showCard("MainMenuCard");
    }

    public void pause() {
        modelUpdateTimer.stop();
        mainFrame.showPauseMenu();
    }

    public void resume() {
        modelUpdateTimer.start();
        mainFrame.hidePauseMenu();
    }
}
