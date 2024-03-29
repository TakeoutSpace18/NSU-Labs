package nsu.urdin.tetris.model.gameplay.listeners;

public interface GameplayStateListener {
    void levelChanged(int level);
    void speedChanged(int delayBetweenSteps);
    void scoreChanged(int score);
    void onGameOver(int finalScore);
}
