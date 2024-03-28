package nsu.urdin.tetris.model;

import lombok.SneakyThrows;
import nsu.urdin.tetris.model.exceptions.CantReadScoreboardFileException;
import nsu.urdin.tetris.model.listeners.ScoreboardListener;

import java.io.*;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

public class Scoreboard implements Serializable {
    private static final String SERIALIZED_FILENAME = ".tetris_scoreboard.dat";

    public record Entry(LocalDateTime date, Integer score) implements Serializable {}
    Entry bestEntry;
    List<Entry> entries;
    transient List<ScoreboardListener> listeners;
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
        listeners.forEach(lis -> lis.scoreTableChanged(entries));
        if (bestEntry != null) {
            listeners.forEach(lis -> lis.highScoreChanged(bestEntry));
        }
    }

    public static Scoreboard readFromFile() throws CantReadScoreboardFileException {
        try (ObjectInputStream in = new ObjectInputStream(new FileInputStream(SERIALIZED_FILENAME))) {
            var scoreboard = (Scoreboard) in.readObject();
            scoreboard.listeners = new ArrayList<>();
            return scoreboard;
        } catch (ClassNotFoundException | IOException e) {
            throw new CantReadScoreboardFileException();
        }
    }

    @SneakyThrows
    public void saveToFile() {
        try (var out = new ObjectOutputStream(new FileOutputStream(SERIALIZED_FILENAME))){
            out.writeObject(this);
        } catch (IOException e) {
            System.out.println("Failed to save scoreboard to file!");
            e.printStackTrace();
        }
    }

}
