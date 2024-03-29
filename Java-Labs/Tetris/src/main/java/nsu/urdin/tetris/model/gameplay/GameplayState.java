package nsu.urdin.tetris.model.gameplay;

import nsu.urdin.tetris.model.gameplay.listeners.GameplayStateListener;

import java.util.ArrayList;
import java.util.List;

public class GameplayState {
    private int curLevel;
    private int curScore;
    private boolean isGameRunning;
    private List<GameplayStateListener> listeners;

    public GameplayState() {
        listeners = new ArrayList<>();
        reset();
    }
    public void reset() {
        isGameRunning = false;
        curLevel = 0;
        curScore = 0;
        listeners.forEach(listener -> listener.scoreChanged(curScore));
        listeners.forEach(listener -> listener.levelChanged(curLevel));
        setLevelSpeed();
    }

    public void setRunning() {
        isGameRunning = true;
    }

    public void addScore(int linesCleared) {
        switch (linesCleared) {
            case 0:
                break;
            case 1:
                curScore += 40;
                break;
            case 2:
                curScore += 100;
                break;
            case 3:
                curScore += 300;
                break;
            case 4:
                curScore += 1200;
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
        int delayBetweenSteps = (int) (600 * Math.pow(0.85, curLevel));
        listeners.forEach(lis -> lis.speedChanged(delayBetweenSteps));
    }


    public void addListener(GameplayStateListener listener) {
        listeners.add(listener);
    }

    public void setGameOver() {
        if (!isGameRunning()) {
            throw new RuntimeException("Already in gameover state");
        }
        listeners.forEach(listener -> listener.onGameOver(curScore));
        isGameRunning = false;
    }

    public boolean isGameRunning() {
        return isGameRunning;
    }

    public int getScore() {
        return curScore;
    }
}
