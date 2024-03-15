package nsu.urdin.tetris.model;

import nsu.urdin.tetris.utils.Vec2i;
import nsu.urdin.tetris.view.TetrisFieldListener;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class TetrisFieldImpl implements TetrisField {
    public static final Vec2i DIMENSIONS = Vec2i.of(10, 16);

    private final List<TetrisFieldListener> listeners;
    private final FigureFactory figureFactory;
    private int[][] landedBlocks;
    private TetrisFigure fallingFigure;

    public TetrisFieldImpl() {
        this.listeners = new ArrayList<>();
        figureFactory = new FigureFactory(listeners);
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
        if (checkCanMove(fallingFigure, fallingFigure.getPosition().add(0, 1))) {
            fallingFigure.move(fallingFigure.getPosition().add(Vec2i.of(0, 1)));
        } else {
            addToLandedBlocks(fallingFigure);
            clearFilledLines();
            spawnNewFigure();
        }

        listeners.forEach(TetrisFieldListener::applyChanges);
    }

    private void clearFilledLines() {
        int linesCleared = 0;
        for (int line = DIMENSIONS.y() - 1; line >= 0; --line) {
            if (checkLineFilled(landedBlocks, line)) {
                linesCleared++;

                for (TetrisFieldListener lis : listeners) {
                    lis.removeLineOfBlocks(line);
                }
            }
            else if (linesCleared != 0) {
                for (TetrisFieldListener lis : listeners) {
                    lis.moveLineOfBlocks(line, linesCleared);
                }

                for (int x = 0; x < landedBlocks.length; ++x) {
                    landedBlocks[x][line + linesCleared] = landedBlocks[x][line];
                }
            }
        }
    }

    private boolean checkLineFilled(int[][] blocks, int line) {
        for (int x = 0; x < DIMENSIONS.x(); ++x) {
            if (blocks[x][line] == 0) {
                return false;
            }
        }
        return true;
    }

    private void spawnNewFigure() {
        fallingFigure = figureFactory.getRandom();
        fallingFigure.spawn(Vec2i.of(4, 0), new Random().nextInt(4));
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

    private boolean checkCollision(int[][] blocks, Vec2i pos) {
        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    if (!checkPositionWithinField(pos.add(i, j)) || landedBlocks[pos.x() + i][pos.y() + j] != 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private boolean checkCanMove(TetrisFigure figure, Vec2i newPos) {
        return !checkCollision(figure.getBlocks(), newPos);
    }

    private boolean checkCanRotate(TetrisFigure figure) {
        return !checkCollision(figure.getNextRotation(), figure.getPosition());
    }

    private boolean checkPositionWithinField(Vec2i pos) {
        return pos.x() >= 0 && pos.x() < DIMENSIONS.x() && pos.y() >= 0 && pos.y() < DIMENSIONS.y();
    }

    @Override
    public void moveLeft() {
        Vec2i newPos = fallingFigure.getPosition().add(-1, 0);
        if (checkCanMove(fallingFigure, newPos)) {
            fallingFigure.move(newPos);
            listeners.forEach(TetrisFieldListener::applyChanges);
        }
    }

    @Override
    public void moveRight() {
        Vec2i newPos = fallingFigure.getPosition().add(1, 0);
        if (checkCanMove(fallingFigure, newPos)) {
            fallingFigure.move(newPos);
            listeners.forEach(TetrisFieldListener::applyChanges);
        }
    }

    @Override
    public void rotate() {
        if (checkCanRotate(fallingFigure)) {
            fallingFigure.rotate();
            listeners.forEach(TetrisFieldListener::applyChanges);
        }
    }

    @Override
    public void fastFall() {
        Vec2i newPos = fallingFigure.getPosition();
        while (checkCanMove(fallingFigure, newPos.add(0, 1))) {
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
