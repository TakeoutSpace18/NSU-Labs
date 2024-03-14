package nsu.urdin.tetris.controller;

import lombok.Getter;
import nsu.urdin.tetris.model.TetrisField;
import nsu.urdin.tetris.model.TetrisFieldImpl;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class TetrisController {

    private static final TetrisController INSTANCE = new TetrisController();

    @Getter
    private final TetrisField field;

    private TetrisController() {
        field = new TetrisFieldImpl();
    }

    public static TetrisController getInstance() {
        return INSTANCE;
    }

    public void scheduleModelUpdate() {
        field.start();
        ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
        scheduler.scheduleAtFixedRate(field::nextStep, 400, 400, TimeUnit.MILLISECONDS);
    }

}
