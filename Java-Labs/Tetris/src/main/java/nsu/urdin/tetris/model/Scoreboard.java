package nsu.urdin.tetris.model;

import nsu.urdin.tetris.model.listeners.ScoreboardListener;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

public class Scoreboard {

    public record Entry(LocalDateTime date, int score) {}
    Entry bestEntry;
    List<Entry> entries;
    List<ScoreboardListener> listeners;
    public Scoreboard() {
        this.bestEntry = null;
        this.entries = new ArrayList<>();
        this.listeners = new ArrayList<>();
    }

    public void addEntry(int score) {
        Entry entry = new Entry(LocalDateTime.now(), score);

        if (bestEntry == null || entry.score > bestEntry.score()) {
            bestEntry = entry;
            listeners.forEach(listener -> listener.highScoreChanged(bestEntry));
        }

        entries.add(entry);
        listeners.forEach(listener -> listener.scoreTableChanged(entries));
    }

    public void addListener(ScoreboardListener listener) {
        listeners.add(listener);
    }

}
