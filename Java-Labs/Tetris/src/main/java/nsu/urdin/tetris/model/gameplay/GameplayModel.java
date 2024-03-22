package nsu.urdin.tetris.model.gameplay;

import nsu.urdin.tetris.model.gameplay.listeners.GameplayStateListener;
import nsu.urdin.tetris.model.gameplay.listeners.TetrisFieldListener;

public interface GameplayModel {
    void restart();
    void nextStep();
    void moveLeft();
    void moveRight();
    void rotate();
    void fastFall();
    void addFieldListener(TetrisFieldListener listener);
    void addGameStateListener(GameplayStateListener listener);
}
