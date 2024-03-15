package nsu.urdin.tetris.controller;

import lombok.Getter;
import nsu.urdin.tetris.model.TetrisField;
import nsu.urdin.tetris.model.TetrisFieldImpl;
import nsu.urdin.tetris.view.JTetrisField;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class TetrisController {

    private static final TetrisController INSTANCE = new TetrisController();

    @Getter
    private final TetrisField field;
    private Timer modelUpdateTimer;

    private TetrisController() {
        field = new TetrisFieldImpl();
    }

    public static TetrisController getInstance() {
        return INSTANCE;
    }

    public void scheduleModelUpdate() {
        field.start();

        modelUpdateTimer = new Timer(400, (ActionEvent e) -> {
            field.nextStep();
        });

        modelUpdateTimer.start();
    }

    public void setupKeyBindings(JTetrisField fieldView) {
        fieldView.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, 0), "left");
        fieldView.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, 0), "right");
        fieldView.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, 0), "up");
        fieldView.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, 0), "down");

        fieldView.getActionMap().put("left", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.moveLeft();
            }
        });
        fieldView.getActionMap().put("right", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.moveRight();
            }
        });
        fieldView.getActionMap().put("down", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.fastFall();
            }
        });
        fieldView.getActionMap().put("up", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.rotate();
            }
        });
    }
}
