package nsu.urdin.tetris.model.listeners;

public interface GameStateListener {
    void levelChanged(int level);
    void speedChanged(int delayBetweenSteps);
    void scoreChanged(int score);
    void onGameOver(int finalScore);
}
