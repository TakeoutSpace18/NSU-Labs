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
        if (!checkCollision(fallingFigure.getBlocks(), fallingFigure.getPosition().x(), fallingFigure.getPosition().y() + 1)) {
            fallingFigure.move(fallingFigure.getPosition().add(Vec2i.of(0, 1)));
        } else {
            addToLandedBlocks(fallingFigure);
            spawnNewFigure();
        }

        listeners.forEach(TetrisFieldListener::applyChanges);
    }

    private void spawnNewFigure()
    {
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

    private boolean checkCollision(int[][] blocks, int newX, int newY) {
        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    if (!checkPositionWithinField(newX + i, newY + j) || landedBlocks[newX + i][newY + j] != 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private boolean checkPositionWithinField(int posX, int posY) {
        return posX >= 0 && posX < DIMENSIONS.x() && posY >= 0 && posY < DIMENSIONS.y();
    }

    @Override
    public void addListener(TetrisFieldListener listener) {
        listeners.add(listener);
    }
}
