package nsu.urdin.tetris.model.listeners;

public class GameStateAdapter implements GameStateListener {
    @Override
    public void levelChanged(int level) {    }
    @Override
    public void speedChanged(int delayBetweenSteps) {    }
    @Override
    public void scoreChanged(int score) {    }
    @Override
    public void onGameOver(int finalScore) {    }
}
