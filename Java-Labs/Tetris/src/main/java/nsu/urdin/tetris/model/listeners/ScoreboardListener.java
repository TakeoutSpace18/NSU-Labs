package nsu.urdin.tetris.model.listeners;

import nsu.urdin.tetris.model.Scoreboard;

import java.util.List;

public interface ScoreboardListener {
    void highScoreChanged(Scoreboard.Entry entry);
    void scoreTableChanged(List<Scoreboard.Entry> scoreTable);

}
