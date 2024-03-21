package nsu.urdin.tetris.model;

import nsu.urdin.tetris.model.listeners.GameStateListener;
import nsu.urdin.tetris.model.listeners.TetrisFieldListener;

public interface GameModel {

    void restart();
    void nextStep();
    void moveLeft();
    void moveRight();
    void rotate();
    void fastFall();
    void addFieldListener(TetrisFieldListener listener);
    void addGameStateListener(GameStateListener listener);
}
