package nsu.urdin.tetris.model;

import nsu.urdin.tetris.model.listeners.GameStateListener;

import java.util.ArrayList;
import java.util.List;

public class GameState {
    private int curLevel;
    private int curScore;
    private List<GameStateListener> listeners;

    public GameState() {
        listeners = new ArrayList<>();
        restart();
    }
    public void restart() {
        curLevel = 0;
        curScore = 0;
        listeners.forEach(listener -> listener.scoreChanged(curScore));
        listeners.forEach(listener -> listener.levelChanged(curLevel));
        setLevelSpeed();
    }

    public void addScore(int linesCleared) {
        switch (linesCleared) {
            case 0:
                break;
            case 1:
                curScore += 40 * (curLevel + 1);
                break;
            case 2:
                curScore += 100 * (curLevel + 1);
                break;
            case 3:
                curScore += 300 * (curLevel + 1);
                break;
            case 4:
                curScore += 1200 * (curLevel + 1);
                break;
            default:
                throw new RuntimeException("Unexpected situation: more than 4 lines can't be cleared at once!");
        }
        listeners.forEach(lis -> lis.scoreChanged(curScore));
        checkNextLevel();
    }

    private void checkNextLevel() {
        boolean levelChanged = false;
        while (curScore >= 300 * (curLevel + 1)) {
            curLevel++;
            levelChanged = true;
        }

        if (levelChanged) {
            listeners.forEach(lis -> lis.levelChanged(curLevel));
            setLevelSpeed();
        }
    }

    private void setLevelSpeed() {
        int delayBetweenSteps = (int) (500 * Math.pow(0.9, curLevel));
        listeners.forEach(lis -> lis.speedChanged(delayBetweenSteps));
    }


    public void addListener(GameStateListener listener) {
        listeners.add(listener);
    }
}
