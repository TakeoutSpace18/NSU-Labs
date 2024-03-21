package nsu.urdin.tetris.model;

import nsu.urdin.tetris.view.TetrisFieldListener;

public interface TetrisField {

    void restart();
    void nextStep();
    void moveLeft();
    void moveRight();
    void rotate();
    void fastFall();
    void addListener(TetrisFieldListener listener);
}
