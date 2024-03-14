package nsu.urdin.tetris.model;

import nsu.urdin.tetris.view.TetrisFieldListener;

public interface TetrisField {

    //TODO: consider having one method 'restart()' instead of 'clear()' and 'start()'
    void start();
    void nextStep();
    void clear();
    void addListener(TetrisFieldListener listener);
}
