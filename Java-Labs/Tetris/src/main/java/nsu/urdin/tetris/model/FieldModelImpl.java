package nsu.urdin.tetris.model;

import nsu.urdin.tetris.utils.Vec2i;

public class FieldModelImpl implements FieldModel {
    public static final int WIDTH = 10;
    public static final int HEIGHT = 16;

    private int[][] landedBlocks;
    private Figure fallingFigure;

    public FieldModelImpl() {
        landedBlocks = new int[WIDTH][HEIGHT];
    }

    @Override
    public void start() {
        spawnNewFigure();
    }

    @Override
    public void clear() {
        landedBlocks = new int[WIDTH][HEIGHT];
        fallingFigure = null;
    }

    @Override
    public void nextStep() {
        if (!checkCollision(fallingFigure.getBlocks(), fallingFigure.getPosition().x(), fallingFigure.getPosition().y() + 1)) {
            fallingFigure.setPosition(fallingFigure.getPosition().add(Vec2i.of(0, 1)));
        } else {
            addToLandedBlocks(fallingFigure);
            spawnNewFigure();
        }
    }

    private void spawnNewFigure() {
        fallingFigure = new Figure();
    }

    private void addToLandedBlocks(Figure fallingFigure) {
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
        if (posX < 0 || posX >= WIDTH || posY < 0 || posY >= HEIGHT) {
            return false;
        }

        return true;
    }
}
