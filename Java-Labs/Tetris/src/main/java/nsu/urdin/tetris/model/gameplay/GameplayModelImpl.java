package nsu.urdin.tetris.model.gameplay;

import nsu.urdin.tetris.model.gameplay.listeners.GameplayStateListener;
import nsu.urdin.tetris.utils.Vec2i;
import nsu.urdin.tetris.model.gameplay.listeners.TetrisFieldListener;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class GameplayModelImpl implements GameplayModel {
    public static final Vec2i DIMENSIONS = Vec2i.of(10, 20);

    private final List<TetrisFieldListener> listeners;
    private final FigureFactory figureFactory;
    private final GameplayState gameState;
    private int[][] landedBlocks;
    private TetrisFigure fallingFigure;

    public GameplayModelImpl() {
        this.listeners = new ArrayList<>();
        figureFactory = new FigureFactory(listeners);
        gameState = new GameplayState();
        landedBlocks = new int[DIMENSIONS.x()][DIMENSIONS.y()];
    }

    @Override
    public void restart() {
        gameState.reset();
        clear();
        spawnNewFigure();
        listeners.forEach(TetrisFieldListener::applyChanges);
    }

    private void clear() {
        listeners.forEach(TetrisFieldListener::clear);
        landedBlocks = new int[DIMENSIONS.x()][DIMENSIONS.y()];
        fallingFigure = null;
    }

    @Override
    public void nextStep() {
        if (gameState.isGameOver()) {
            return;
        }

        if (checkCanMove(fallingFigure, fallingFigure.getPosition().add(0, 1))) {
            fallingFigure.move(fallingFigure.getPosition().add(Vec2i.of(0, 1)));
        } else {
            addToLandedBlocks(fallingFigure);
            int linesCleared = clearFilledLines();
            gameState.addScore(linesCleared);
            boolean spawned = spawnNewFigure();
            if (!spawned) {
                gameState.setGameOver();
            }
        }

        listeners.forEach(TetrisFieldListener::applyChanges);
    }

    private int clearFilledLines() {
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

        return linesCleared;
    }

    private boolean checkLineFilled(int[][] blocks, int line) {
        for (int x = 0; x < DIMENSIONS.x(); ++x) {
            if (blocks[x][line] == 0) {
                return false;
            }
        }
        return true;
    }

    private boolean spawnNewFigure() {
        fallingFigure = figureFactory.getRandom();
        int rotation = new Random().nextInt(4);
        if (!checkCollision(fallingFigure.getRotation(rotation), Vec2i.of(4, 0))) {
            fallingFigure.spawn(Vec2i.of(4, 0), rotation);
            return true;
        }
        return false;
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
    public void addFieldListener(TetrisFieldListener listener) {
        listeners.add(listener);
    }

    @Override
    public void addGameStateListener(GameplayStateListener listener) {
        gameState.addListener(listener);
    }
}
