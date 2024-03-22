package nsu.urdin.tetris.model.gameplay.listeners;

public class GameplayStateAdapter implements GameplayStateListener {
    @Override
    public void levelChanged(int level) {    }
    @Override
    public void speedChanged(int delayBetweenSteps) {    }
    @Override
    public void scoreChanged(int score) {    }
    @Override
    public void onGameOver(int finalScore) {    }
}
