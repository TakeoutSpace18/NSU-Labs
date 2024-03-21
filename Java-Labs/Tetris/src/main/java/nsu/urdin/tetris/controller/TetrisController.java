package nsu.urdin.tetris.controller;

import nsu.urdin.tetris.view.JMainFrame;
import nsu.urdin.tetris.model.TetrisField;
import nsu.urdin.tetris.model.TetrisFieldImpl;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;

public class TetrisController {
    private static final TetrisController INSTANCE = new TetrisController();

    private final TetrisField field;
    private JMainFrame mainFrame;
    private Timer modelUpdateTimer;

    private TetrisController() {
        field = new TetrisFieldImpl();
    }

    public void launch() {
        mainFrame = new JMainFrame();

        field.addListener(mainFrame.getTetrisFieldListener());
        setupKeyBindings(mainFrame.getRootPane());
    }

    public static TetrisController getInstance() {
        return INSTANCE;
    }

    public void scheduleModelUpdate() {
        modelUpdateTimer = new Timer(400, (ActionEvent e) -> {
            field.nextStep();
        });

        modelUpdateTimer.start();
    }

    public void setupKeyBindings(JComponent component) {
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, 0), "left");
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, 0), "right");
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, 0), "up");
        component.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, 0), "down");

        component.getActionMap().put("left", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.moveLeft();
            }
        });
        component.getActionMap().put("right", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.moveRight();
            }
        });
        component.getActionMap().put("down", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.fastFall();
            }
        });
        component.getActionMap().put("up", new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                field.rotate();
            }
        });
    }

    public void newGame() {
        field.restart();
        scheduleModelUpdate();

        mainFrame.showGamePanel();
    }
}
