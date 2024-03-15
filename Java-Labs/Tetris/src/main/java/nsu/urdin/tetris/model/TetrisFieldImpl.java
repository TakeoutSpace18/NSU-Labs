package nsu.urdin.tetris.model;

import nsu.urdin.tetris.utils.Vec2i;
import nsu.urdin.tetris.view.TetrisFieldListener;

import java.util.ArrayList;
import java.util.List;

public class TetrisFieldImpl implements TetrisField {
    public static final Vec2i DIMENSIONS = Vec2i.of(10, 16);

    private final List<TetrisFieldListener> listeners;
    private int[][] landedBlocks;
    private TetrisFigure fallingFigure;

    public TetrisFieldImpl() {
        this.listeners = new ArrayList<>();
        landedBlocks = new int[DIMENSIONS.x()][DIMENSIONS.y()];
    }

    @Override
    public void start() {
        spawnNewFigure();
        listeners.forEach(TetrisFieldListener::applyChanges);
    }

    @Override
    public void clear() {
        landedBlocks = new int[DIMENSIONS.x()][DIMENSIONS.y()];
        fallingFigure = null;
    }

    @Override
    public synchronized void nextStep() {
        if (!checkCollision(fallingFigure, fallingFigure.getPosition().add(0, 1))) {
            fallingFigure.move(fallingFigure.getPosition().add(Vec2i.of(0, 1)));
        } else {
            addToLandedBlocks(fallingFigure);
            spawnNewFigure();
        }

        listeners.forEach(TetrisFieldListener::applyChanges);
    }

    private void spawnNewFigure() {
        fallingFigure = new TetrisFigure(listeners);
    }

    private void addToLandedBlocks(TetrisFigure fallingFigure) {
        int[][] blocks = fallingFigure.getBlocks();

        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    landedBlocks[fallingFigure.getPosition().x() + i][fallingFigure.getPosition().y() + j] = blocks[i][j];
                }
            }
        }
    }

    private boolean checkCollision(TetrisFigure figure, Vec2i newPos) {
        int[][] blocks = figure.getBlocks();

        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    if (!checkPositionWithinField(newPos.add(i, j)) || landedBlocks[newPos.x() + i][newPos.y() + j] != 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private boolean checkPositionWithinField(Vec2i pos) {
        return pos.x() >= 0 && pos.x() < DIMENSIONS.x() && pos.y() >= 0 && pos.y() < DIMENSIONS.y();
    }

    void tryMoveFallingFigure(Vec2i newPos) {
        if (!checkCollision(fallingFigure, newPos)) {
            fallingFigure.move(newPos);
            listeners.forEach(TetrisFieldListener::applyChanges);
        }
    }

    @Override
    public void moveLeft() {
        Vec2i newPos = fallingFigure.getPosition().add(-1, 0);
        tryMoveFallingFigure(newPos);
    }

    @Override
    public void moveRight() {
        Vec2i newPos = fallingFigure.getPosition().add(1, 0);
        tryMoveFallingFigure(newPos);
    }

    @Override
    public void fastFall() {
        Vec2i newPos = fallingFigure.getPosition();
        while (!checkCollision(fallingFigure, newPos.add(0, 1))) {
            newPos = newPos.add(0, 1);
        }
        fallingFigure.move(newPos);
        listeners.forEach(TetrisFieldListener::applyChanges);
    }

    @Override
    public void addListener(TetrisFieldListener listener) {
        listeners.add(listener);
    }
}
