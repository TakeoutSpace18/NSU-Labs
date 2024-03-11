package nsu.urdin.tetris.model;

public interface FieldModel {

    //TODO: consider having one method 'restart()' instead of 'clear()' and 'start()'
    void start();
    void nextStep();
    void clear();
}
